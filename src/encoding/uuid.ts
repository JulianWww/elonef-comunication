import { randomUUID } from "crypto";
import { Buffer } from "buffer";

/**
 * For efficency reasons the UUIDv4 is converted to a binary buffer and not transmitted with hexadecimansl
 * @returns the UUID in binary form
 */
function uuid() {
    return Buffer.from(randomUUID().replaceAll("-", ""), "hex");
}

/**
 * the bytelength of the uuid
 */
const uuid_size = 16;

export default uuid
export { uuid_size }