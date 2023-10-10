import assert from "assert";
import { Buffer } from "buffer";

/**
 * A handler that will permit us to read a Buffer n bytes at a time. This is used by the decoding system to make it simpler to understand
 * @type Class
 */
class BufferReader {
    /**
     * The buffer that is beeing read from
     */
    private readonly buff: Buffer;

    /**
     * The index of the reading head.
     */
    private idx: number

    /**
     * @param buff The buffer to be read. 
     */
    constructor(buff: Buffer) {
        this.buff = buff;
        this.idx = 0;
    }

    /**
     * reads the next (amount) bytes from the buffer
     * @param amount the amount of bytes to read
     * @returns the nest (amount) of bytes as a buffer
     */
    read(amount: number=1) {
        assert(this.idx + amount <= this.buff.length, new Error("Buffer overread, out of buffer length"));
        assert(amount > 0, new Error("amount must be positive"))

        const out = this.buff.subarray(this.idx, this.idx + amount);
        this.idx = this.idx + amount;
        return out;
    }

    /**
     * reads all the remaining data in the Buffer
     * @returns whatever data is left in the buffer.
     */
    readRest() {
        return this.buff.subarray(this.idx);
    }

    /**
     * True only if there is more data to read
     * @returns true if there is more data to be read else false.
     */
    canRead() {
        return this.idx < this.buff.length;
    }
}

export default BufferReader