import { v4 } from "uuid";
import { Buffer } from "buffer";
import { randomBytes } from "../encription/aes"

/**
 * For efficency reasons the UUIDv4 is converted to a binary buffer and not transmitted with hexadecimansl
 * @returns the UUID in binary form
 */
function uuid() {
    try {
        return Buffer.from(v4().replaceAll("-", ""), "hex");
    }
    catch {
        return randomBytes(uuid_size)
    }
}

/**
 * the bytelength of the uuid
 */
const uuid_size = 16;

export default uuid
export { uuid_size }