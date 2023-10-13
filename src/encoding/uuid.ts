import { v4 } from "uuid";
import { Buffer } from "buffer";

/**
 * For efficency reasons the UUIDv4 is converted to a binary buffer and not transmitted with hexadecimansl
 * @returns the UUID in binary form
 */
function uuid() {
    return Buffer.from(v4().replaceAll("-", ""), "hex");
}

/**
 * the bytelength of the uuid
 */
const uuid_size = 16;

export default uuid
export { uuid_size }