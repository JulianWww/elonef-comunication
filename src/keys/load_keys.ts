import { nothing } from "../types";
import forge from "node-forge";


function import_private(key_data: string | nothing){
    if (!key_data) {
        return undefined
    }
    if (key_data[0] === "-") {
        return forge.pki.privateKeyFromPem(key_data);
    }
    return Buffer.from(key_data, "base64")
}

function import_public(key_data: string | nothing) {
    if (!key_data) {
        return undefined
    }
    if (key_data[0] === "-") {
        return forge.pki.publicKeyFromPem(key_data);
    }
    return Buffer.from(key_data, "base64")
}

export { import_private, import_public }