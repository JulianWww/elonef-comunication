class SecutiryError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "SecutiryError";
    }
}

class DataModificationError extends SecutiryError {
    constructor(message: string) {
        super(message);
        this.name = "DataModificationError"
    }
}

class ForwardedError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "ForwardedError"
    }
}

class RemoteError extends Error {
    constructor(message: string) {
        super(message);
        this.name = "RemoteError"
    }
}

export { SecutiryError, DataModificationError, ForwardedError, RemoteError }