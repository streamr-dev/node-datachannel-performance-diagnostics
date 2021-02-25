#!/usr/bin/env ts-node
import * as nodeDataChannel from "node-datachannel"
import * as WebSocket from "ws"
import { DataChannel } from "node-datachannel"

const ICE_SERVERS = ['stun:stun.l.google.com:19302']

const BUFFER_LOW = 2 ** 20
const BUFFER_HIGH = 2 ** 24

function randomString(length: number): string {
    let result = ''
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'
    const charactersLength = characters.length
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength))
    }
    return result
}

class PeerConnection {
    private readonly selfId: string
    private readonly peerId: string
    private readonly logId: string
    private readonly connection: nodeDataChannel.PeerConnection
    private dc: nodeDataChannel.DataChannel | null
    private paused = true
    private bytesIn = 0
    private bytesOut = 0

    constructor(selfId: string, peerId: string, signalingServerWs: WebSocket) {
        this.selfId = selfId
        this.peerId = peerId
        this.logId = `${this.selfId}->${this.peerId}`
        function sendRelay(data) {
            signalingServerWs.send(JSON.stringify({
                type: "relay",
                to: peerId,
                data
            }))
        }
        this.connection = new nodeDataChannel.PeerConnection(selfId, { iceServers: ICE_SERVERS })
        // TODO: connection.onStateChange
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
        this.setUpDataChannel(this.connection.createDataChannel('generalDataChannel'))
        this.dc.onOpen(() => {
            this.paused = false
        })
    }

    startAsPassive(): void {
        if (this.dc) {
            throw new Error('Already started!')
        }
        this.connection.onDataChannel((dc) => {
            this.setUpDataChannel(dc)
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
        if (this.paused) {
            return false
        }
        if (this.dc.bufferedAmount() >= BUFFER_HIGH) {
            this.paused = true
            console.info(`${this.logId} DataChannel HIGH buffer (${this.dc.bufferedAmount()})!`)
            return false
        }
        const success = this.dc.sendMessage(message)
        if (success) {
            this.bytesOut += message.length
        }
        return success
    }

    getLogId(): string {
        return this.logId
    }

    getBytesIn(): number {
        return this.bytesIn
    }

    getBytesOut(): number {
        return this.bytesOut
    }

    resetCounters(): void {
        this.bytesIn = 0
        this.bytesOut = 0
    }

    private setUpDataChannel(dc: DataChannel): void {
        dc.setBufferedAmountLowThreshold(BUFFER_LOW)
        dc.onOpen(() => {
            console.info(`${this.logId} DataChannel ${this.peerId} open`)
        })
        dc.onClosed(() => {
            console.warn(`${this.logId} DataChannel ${this.peerId} closed`)
        })
        dc.onError((e) => {
            console.warn(`${this.logId} DataChannel ${this.peerId} error: ${e}`)
        })
        dc.onBufferedAmountLow(() => {
            this.paused = false
            console.info(`${this.logId} DataChannel ${this.peerId} LOW buffer (${this.dc.bufferedAmount()})!`)
        })
        dc.onMessage((msg) => {
            this.bytesIn += msg.length
        })
        this.dc = dc
    }
}

export default function startClient(id: string, wsUrl: string) {
    const connections: { [key: string]: PeerConnection } = {}
    const ws = new WebSocket(`${wsUrl}?id=${id}`)

    ws.on('open', () => {
        console.info("Connected to signaling server; waiting for further messages from signaling server...")
    })
    ws.on("message", (rawMsg) => {
        const msg = JSON.parse(rawMsg.toString())
        if (msg.type === 'connect') {
            console.info(`Connect message received from signaling server (target = ${msg.target}).`)
            connections[msg.target] = new PeerConnection(id, msg.target, ws)
            connections[msg.target].startAsActive()
        } else if (msg.type === 'relay') {
            console.info(`Relay message received signaling server (from=${msg.from}).`)
            if (!connections[msg.from]) {
                console.info(`Creating passive connection for ${msg.from}.`)
                connections[msg.from] = new PeerConnection(id, msg.from, ws)
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

    setInterval(() => {
        const msg = randomString(600)
        Object.values(connections).forEach((conn) => {
            conn.publish(msg)
        })
    }, 5)

    setInterval(() => {
        let totalIn = 0
        let totalOut = 0
        Object.values(connections).forEach((conn) => {
            totalIn += conn.getBytesIn()
            totalOut += conn.getBytesOut()
            console.info(`${conn.getLogId()} rate ${conn.getBytesIn() / 1024} / ${conn.getBytesOut() / 1024} kb`)
            conn.resetCounters()
        })
        console.info(`Total ${totalIn / 1024} / ${totalOut / 1024} kb`)
    }, 1000)
}

const WS_URL = "ws://localhost:8080/"
const clientArgs = process.argv.slice(2)
startClient(clientArgs.length > 0 ? clientArgs[0] : 'client-' + randomString(4), WS_URL)
