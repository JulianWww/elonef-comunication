const { encript_rsa, decript_rsa, generateKeyPairRSA, import_private, import_public } = require("../lib/index.js");

function generate_key() {
    return generateKeyPairRSA();
}
var {publicKey, privateKey} = generate_key()

function test_load_key_rsa(txt) {
    const priv = import_private(privateKey)
    const pub = import_public(publicKey)

    const dec = decript_rsa(encript_rsa(txt, priv), pub)
    return txt === dec
    
}

test(
    "load_pub_key", () => {
        expect(()=> {
            import_public(publicKey)
        }).toBeTruthy();
    }
)

test(
    "load_priv_key", () => {
        expect(()=> {
            import_private(privateKey)
        }).toBeTruthy();
    }
)

test(
    "load_priv_key", () => {
        expect(()=> 
            test_load_key_rsa("the moon came falling down onto the boor unsuspecting wm15.")
        ).toBeTruthy();
    }
)