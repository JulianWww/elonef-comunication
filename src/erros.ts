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

export { SecutiryError, DataModificationError }