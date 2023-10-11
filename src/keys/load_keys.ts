import { nothing } from "../types";
import NodeRSA, { Format } from "node-rsa";


function import_private(key_data: string | nothing, encoding: Format="pkcs1"){
    console.log(key_data, encoding)
    if (!key_data) {
        return undefined
    }
    if (key_data[0] === "-") {
        const rsa = new NodeRSA({b: 2048})
        return rsa.importKey(key_data, encoding)
    }
    return Buffer.from(key_data, "base64")
}

function import_public(key_data: string | nothing) {
    return import_private(key_data, "pkcs1-public")
}

export { import_private, import_public }