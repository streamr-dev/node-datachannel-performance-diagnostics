import * as nodeDataChannel from "node-datachannel"
import * as WebSocket from "ws"
import { DataChannel } from "node-datachannel"

const ICE_SERVERS = ['stun:stun.l.google.com:19302']


const BYTES_TO_SEND = 1048576 * 100     // 100 megabytes


function randomString(length: number): string {
    let result = ''
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'
    const charactersLength = characters.length
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength))
    }
    return result
}

function formatRate(bytes: number, timeElapsedMilliseconds: number): string {
    let bytesPerSecond = bytes / timeElapsedMilliseconds * 1000;
    return `${Math.round(bytesPerSecond / 1024)}`
}

class PeerConnection {
    private readonly selfId: string
    private readonly peerId: string
    private readonly logId: string
    private readonly connection: nodeDataChannel.PeerConnection
    private dc: nodeDataChannel.DataChannel | null
    private paused = false
    private lastState = "unknown"
    private bytesIn = 0
    private bytesOut = 0
    private bytesFailed = 0
    private message: string 
    private isActive = false
    private BUFFER_LOW: number 
    private BUFFER_HIGH: number

    // key is the packet size
    private stats: Map<number, {fistPacketAt: number, lastPacketAt: number, bytesReceived: number}> = new Map()

    constructor(selfId: string, peerId: string, signalingServerWs: WebSocket, packetSize: number) {
        this.selfId = selfId
        this.peerId = peerId
        this.logId = `${this.selfId}->${this.peerId}`
        this.message = randomString(packetSize)
        this.BUFFER_LOW = packetSize
        this.BUFFER_HIGH = packetSize * 10

        function sendRelay(data) {
            signalingServerWs.send(JSON.stringify({
                type: "relay",
                to: peerId,
                data
            }))
        }
        this.connection = new nodeDataChannel.PeerConnection(selfId, { iceServers: ICE_SERVERS })
        this.connection.onStateChange((state) => {
            this.lastState = state
        })
        // TODO: connection.onGatheringStateChange
        this.connection.onLocalDescription((description, type) => {
            sendRelay({
                type,
                description
            })
        })
        this.connection.onLocalCandidate((candidate, mid) => {
            sendRelay({
                candidate,
                mid
            })
        })
    }

    startAsActive(): void {
        if (this.dc) {
            throw new Error('Already started!')
        }
        this.isActive = true
        this.dc = this.connection.createDataChannel('generalDataChannel')
        this.setUpDataChannel(this.dc)
    }

    startAsPassive(): void {
        if (this.dc) {
            throw new Error('Already started!')
        }
        this.connection.onDataChannel((dc) => {
            this.dc = dc
            this.setUpDataChannel(this.dc)
            this.paused = false
        })
    }

    handleRemoteData(data: any) {
        if (data.candidate) {
            this.connection.addRemoteCandidate(data.candidate, data.mid)
        } else if (data.type) {
            this.connection.setRemoteDescription(data.description, data.type)
        } else {
            console.warn(`${this.logId} unrecognized RTC message: ${JSON.stringify(data)}`)
        }
    }

    publish(message: string): boolean {
        /*
        if (this.paused) {
            this.bytesFailed += message.length
            return false
        }
        if (this.dc.bufferedAmount() >= BUFFER_HIGH) {
            this.paused = true
            console.info(`${this.logId} DataChannel HIGH buffer (${this.dc.bufferedAmount()})!`)
            this.bytesFailed += message.length
            return false
        }
        */
        while (this.dc.bufferedAmount() < this.BUFFER_HIGH) {
        //console.log(`Buffered amount in busy sending loop ${this.dc.bufferedAmount()}`)
        const success = this.dc.sendMessage(message)
        this.bytesOut += message.length
        }
        console.log("busy sending loop exited, buf: " + this.dc.bufferedAmount())
        return true
    }

    getLogId(): string {
        return this.logId
    }

    getBufferedAmount(): number {
        return this.dc ? this.dc.bufferedAmount() as number : 0
    }

    getState(): string {
        return this.lastState
    }

    isOpen(): boolean {
        return this.dc ? this.dc.isOpen() : false
    }

    isPaused(): boolean {
        return this.paused
    }

    getBytesIn(): number {
        return this.bytesIn
    }

    getBytesOut(): number {
        return this.bytesOut
    }

    getBytesFailed(): number {
        return this.bytesFailed
    }

    resetCounters(): void {
        this.bytesIn = 0
        this.bytesOut = 0
        this.bytesFailed = 0
    }

    getStatsAsString(): string {
        let ret = "";
        var keys = [...this.stats.keys()]
        for (let i = 0; i < keys.length; i++) {
            let packetSize = keys[i];    
            let stat = this.stats.get(packetSize)
            const timeSpent = stat.lastPacketAt - stat.fistPacketAt
            
            let bytesPerSecond = stat.bytesReceived / timeSpent * 1000

            const kilobytesPerSecond = Math.round(bytesPerSecond / 1024) 
            ret += packetSize + "\t" + kilobytesPerSecond + " kB/s \t" + (kilobytesPerSecond*8) + " kbit/s\n"
          }
        return ret
    } 

    private setUpDataChannel(dc: DataChannel): void {
       
        dc.setBufferedAmountLowThreshold(this.BUFFER_LOW)
        dc.onOpen(() => {
            console.info(`${this.logId} DataChannel ${this.peerId} open`)
            if (this.isActive) {
                this.publish(this.message)
            }
        })
        dc.onClosed(() => {
            console.warn(`${this.logId} DataChannel ${this.peerId} closed`)
        })
        dc.onError((e) => {
            console.warn(`${this.logId} DataChannel ${this.peerId} error: ${e}`)
        })
        if (this.isActive) {
            console.log("Setting bufferedAmountLow callback as active")
            dc.onBufferedAmountLow(() => {
                console.log("!!!! onBufferedAmountLow")
                //console.log(`${this.logId} DataChannel ${this.peerId} LOW buffer (${this.dc.bufferedAmount()})!`)
                //this.paused = false 
                this.publish(this.message);
            })
        }
        dc.onMessage((msg) => {
            if (!this.stats.has(msg.length)) {
                this.stats.set(msg.length, {fistPacketAt: Date.now(), lastPacketAt: Date.now(), bytesReceived: msg.length})
            }
            else {
                let stat = this.stats.get(msg.length);
                stat.bytesReceived += msg.length;
                stat.lastPacketAt = Date.now();
                this.stats.set(msg.length, stat);
            }
            this.bytesIn += msg.length
        })
        
    }
}

const connections: { [key: string]: PeerConnection } = {} // peerId => Rtc connection

export default function startClient(id: string, wsUrl: string, packetSize: number) {

    const ws = new WebSocket(`${wsUrl}?id=${id}`) // connection to signaling server

    ws.on('open', () => {
        console.info("Connected to signaling server; waiting for further messages from signaling server...")
    })
    ws.on("message", (rawMsg) => {
        const msg = JSON.parse(rawMsg.toString())
        if (msg.type === 'connect') {
            console.info(`Connect message received from signaling server (target = ${msg.target}).`)
            connections[msg.target] = new PeerConnection(id, msg.target, ws, packetSize)
            connections[msg.target].startAsActive()
        } else if (msg.type === 'relay') {
            console.info(`Relay message received signaling server (from=${msg.from}).`)
            if (!connections[msg.from]) {
                console.info(`Creating passive connection for ${msg.from}.`)
                connections[msg.from] = new PeerConnection(id, msg.from, ws, packetSize)
                connections[msg.from].startAsPassive()
            }
            connections[msg.from].handleRemoteData(msg.data)
        } else {
            console.warn(`Unrecognized message type ${msg.type} from signaling server.`)
        }
    })
    ws.on('close', () => {
        console.error('Connection lost to signaling server')
        process.exit(1)
    })

    // Publish message to all neighbors
    /*
    const publishFn = () => {
        const msg = randomString(PACKAGE_SIZE)
        Object.values(connections).forEach((conn) => {
            conn.publish(msg)
        })
        setTimeout(publishFn, INTERVAL)
    }
    setTimeout(publishFn, INTERVAL)
    */

    let lastIntervalTime = Date.now();

    // Print to console statistics
    setInterval(() => {
        let totalIn = 0
        let totalOut = 0
        let totalFailed = 0
        let totalBufferedAmount = 0   
        let timeElapsed = Date.now() - lastIntervalTime     

        Object.values(connections).forEach((conn) => {
            totalIn += conn.getBytesIn()
            totalOut += conn.getBytesOut()
            totalFailed += conn.getBytesFailed()
            totalBufferedAmount += conn.getBufferedAmount()

            console.info(`${conn.getLogId()} rate ${formatRate(conn.getBytesIn(), timeElapsed )} / ${formatRate(conn.getBytesOut(), timeElapsed)} kB/s`
                + ` (${formatRate(conn.getBytesFailed(), timeElapsed)}, ${formatRate(conn.getBufferedAmount(), timeElapsed)}, state=${conn.getState()}, open=${conn.isOpen()}, paused=${conn.isPaused()})`)
            conn.resetCounters()
        })
        const memoryUsage = process.memoryUsage()
        console.info(`Total ${formatRate(totalIn, timeElapsed)} / ${formatRate(totalOut, timeElapsed)} kB/s (${totalFailed/1024}, ${totalBufferedAmount/1024}, ${memoryUsage.heapUsed} / ${memoryUsage.heapTotal})\n`)
        lastIntervalTime = Date.now();
    }, 1000)
}

function printStats() {
    Object.values(connections).forEach((conn) => {
        console.log("packetSize kB/s    kbit/s");
        console.log(conn.getStatsAsString())
    })
}
process.on('SIGINT', function() {
    console.log('Caught interrupt signal');
    printStats()

    process.exit();
});

const WS_URL = process.env.WS_URL || "ws://localhost:8080/"
const clientArgs = process.argv.slice(2)

let packetSize = 800;
if (clientArgs.length > 0) {
    packetSize = parseInt(clientArgs[0]);
}
startClient('client-' + randomString(4), WS_URL, packetSize)
