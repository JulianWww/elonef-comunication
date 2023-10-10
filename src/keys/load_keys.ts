import { createPrivateKey, createPublicKey } from "crypto";
import { nothing } from "../types";
import { KeyObject } from "../types";


function import_private(key_data: string | KeyObject ){
    if (typeof key_data !== "string") {
        return key_data;
    }
    return createPrivateKey(key_data);
}

function import_public(key_data: string | KeyObject | nothing) {
    if (! key_data) {
        return null;
    }
    if (typeof key_data !== "string") {
        return key_data;
    }
    return createPublicKey(key_data);
}

export { import_private, import_public }