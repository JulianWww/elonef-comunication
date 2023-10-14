import { generatePrivate, getPublic, derive } from "eccrypto";
import { Buffer } from "buffer";

function get_curve() {
    return generatePrivate()
}


/**
 * Builds a ecdh key on the client side. This will send a message to the server that must be routed to `reciever_dh_key_build`
 * the sender function takes the data in string format and implements the routing to `reciever_dh_key_build`. I must be implemented
 * by the developer themselves. The return must be the responce from `reciever_dh_key_build`
 * @param id {string} - the exchange id so that the server can find the right key in its cache.
 */
async function initalize_dh_key_build(sender: (toSend: Buffer) => Promise<Buffer>) {
    const priv_key = get_curve();
    const other_public_key = await sender(getPublic(priv_key))
    return derive(priv_key, other_public_key)
}

/**
 * Performs the key exchange on the server side. See `initalize_dh_key_build` for instructions on where to put this in code.
 * it return is public key that must be passed as the return of the `initalize_dh_key_build` sender function
 * @param other_key {string} - the data sent by the client
 * @param secret_sing {(secret: Buffer) => void} - a sink that takes the shared secret
 */
function reciever_dh_key_build(other_key: Buffer, secret_sink: (secret: Buffer) => void) {
    const priv_key = get_curve();

    derive(
        priv_key,
        other_key,
    ).then(secret_sink)
    
    return getPublic(priv_key);
}

export { initalize_dh_key_build, reciever_dh_key_build }