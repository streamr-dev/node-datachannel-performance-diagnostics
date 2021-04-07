import * as WebSocket from 'ws'
import * as url from "url"

const wss = new WebSocket.Server({ port: 8080 })

const clients: { [key: string]: WebSocket } = {} // peerId => connection

wss.on('connection', (ws, req) => {
    const params = url.parse(req.url, true) // Query parameter: ?id=client-1
    const { id } = params.query

    if (!id) {
        console.warn("Terminated connection, no 'id' param set.")
        ws.terminate()
    }

    const peerId = id as string
    console.info(`Peer ${peerId} connected.`)

    ws.on('message', (rawMsg) => {
        const msg = JSON.parse(rawMsg.toString())
        if (msg.type === "relay") { // WebRTC signalling message, e.g., answer / offer / description
            let otherPeerId = msg.to
            let otherPeerWs = clients[otherPeerId]
            console.info(`Relay message from ${peerId} to ${otherPeerId}: ${msg}.`)

            if (!otherPeerWs) {
                console.warn(`Peer ${otherPeerId} not found.`)
            } else {
                otherPeerWs.send(JSON.stringify({
                    type: 'relay',
                    from: id,
                    to: otherPeerId,
                    data: msg.data
                }))
            }
        } else {
            console.warn(`Unrecognized message type ${msg.type} from ${peerId}.`)
        }
    })

    ws.on('close', () => {
        console.info(`Peer ${peerId} disconnected.`)
        delete clients[peerId]
    })

    // Connect new client to all existing clients (thereby leading to a complete graph)
    Object.entries(clients).forEach(([otherPeerId, otherPeerWs]) => {
        const offering = false //Math.random() < 0.5
        if (offering) {
            ws.send(JSON.stringify({
                type: 'connect',
                target: otherPeerId
            }))
        } else {
            otherPeerWs.send(JSON.stringify({
                type: 'connect',
                target: id
            }))
        }
    })

    clients[peerId] = ws
})

