const { ServerConnectionHandler, generateClientKeys } = require('../lib');

const key = {
  "data_key": {
    "key": "-----BEGIN RSA PUBLIC KEY-----\nMIIBCgKCAQEAs0GiTYqcw7NFkWEZXUMGGh8pDc2HUwYx+60jDa0Af6R1VGu+KZiy\nA6bXKPDUmCoRftvhOuGaOq8d8FL28ybEW/xBOsIAtJtIN4ZoD0ME0VvTCMm1Vz8w\nVqYrI0vFftlZoslKqRwKXe2cPIEBzkB6fJrwsAkUczxowzc7N9DHqWd4CESJdpzQ\n4ijr2oi0YhytWKNpjTwi9WapS2xNUbYMRquxP2QVAFvKg4nO+jtZkxMFMAelhuxo\n2AQ8/cBAUw8uDI+jfop4OUux21xhUvkHgG0i1OObsAulV8LfqTFlE+tnlzKfzfyU\nS2XTxXRiuiUMLCs9nlSgLBxkqpfJbPBJIwIDAQAB\n-----END RSA PUBLIC KEY-----",
    "signatures": {}
  },
  "sign_key": {
    "key": "BI2VJkAembsuvCvhyqIx2ZWs29ZW7/3/HcHlXd2pLK+tZv5YEnDzT6apjfkaCr5K39dhaJAzUXxghRjVDVn95MY=",
    "signatures": {}
  }
}


console.log(generateClientKeys("", [], "d").then(key => console.log(JSON.stringify(key, null, 2))))

var chat_key = new Map();
var messages = [];
var chat_key_id = ""

const wss = new ServerConnectionHandler(
    { port: 8080 }, 
    id => key,
    (chat_id, key_id) => chat_key.get(key_id),
    (chat_id) => chat_key_id,
    (chat_id, key_id, keys) => {
      chat_key.set(key_id, keys);
      chat_key_id = key_id;
    },
    (chat_id, message) => {
      messages.push(message);
    },
    (chat_id, last_idx, lenght) => messages
  )
  .add_api_callback("test/hi", (data, uid) => {
    console.log(data.toString(), uid);
    return Buffer.from("return data");
  });