/**
 * Base error raised for security issues
 */
class SecutiryError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "SecutiryError";
    }
}

/**
 * raised when data has been modified and the signaure nolonger matches the data
 */
class DataModificationError extends SecutiryError {
    constructor(message: string) {
        super(message);
        this.name = "DataModificationError"
    }
}

/**
 * raised of you want the error to be forwarded to the client
 */
class ForwardedError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "ForwardedError"
    }
}

/**
 * Raised on the near side of the connection when an error occurs on the remote end
 */
class RemoteError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "RemoteError"
    }
}

export { SecutiryError, DataModificationError, ForwardedError, RemoteError }