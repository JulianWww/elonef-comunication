const { ClientConnectionHandler } = require('../lib');

const key = {
  "sign_key": "-----BEGIN EC PRIVATE KEY-----\nMIHuAgEBBEgBFRSmXoOiK8mv5tCtqhlthzRq3qr++d01uNFUg1ZVJIjD61yhz+kd\n8RRWju/FlrZNtmcN2MV+noxpCtlcv29Y4AxFsTgDeaqgBwYFK4EEACehgZUDgZIA\nBADCOdTeexFBSd48npTOw7S3wFf24rIsFZlxgj4YR8ED9GNKHc9QwusU0zpa0dLu\nCtW+Ajjdia8xcOVT4WLFdo04ieQTvaityAKXqFcqKahN8i4m4IDUcWECmUlJHNHG\nPOG1pP8bfHNNs12+gvfSiRluy3omuqH5ilB0EUPURBpRofNLFqCSV1x8YHzYTU87\n1Q==\n-----END EC PRIVATE KEY-----\n",
  "data_key": "-----BEGIN RSA PRIVATE KEY-----\nMIICXwIBAAKBgQDcEi6RFAxDs5Oay9Vc19Kk6cLqTS7srCDtwGsSZsiLtZ/noSa1\n9De9ar3vSut7THavWocbl3l9xXLuysiPRFcfxr/GqxOCReOUPB2UdzFgxVqtGHtD\nPZfuuN4NZgKJDIK8160aWD4RH9/wFnWAnc04eKz6N/kLW0VLn0XE/2Xn4QIDAQAB\nAoGBAJ5upIGjijTPOkttxTwi0C4TcFSXnlmajdY7+Zl0z5ns9qKd3K2PlPVTvEti\nh2brpGIXbT8K7NcD5/tLl9m0XHdYqDzzLfZ/V98RG3L1/SlaFjBI7xYtEU0K0cDq\nwdb98qpq3noxvkukmPmRXcsm2EBBxSjCjBGuoaICj6bzg3I1AkEA+unrHQLNDAng\nutmUQWh0Phf0NYJZsc0VVGSMVkxs8ip0ha41B1ffHHoCrYwMYwZcH+S34Yewyopf\n/qAPGdz+lwJBAOCINaOcrpPb3ox/+RrubQ5YvnVW+Er4yjA8K8pWQyuuuIJNm/Cw\njuDb3psnZ383kpplRBHz0ZALdvE1Kdj0lEcCQQC1UlWVoXCMQpckcBezLMjUFntk\nL86hrpK7ae4AbzpLxmJuOHYZ2GZjLcjBoXNSHEZ2xdIcKPKZa2Xzm7dN+WYBAkEA\nz+bvm9Z5g+ljkT7x3rEEg/NCL45fC7SAKtecSJdFTQ5URDsueFhTg/uyjIGZAJ/p\nfFLmC6Vy1uj2snPiksQtAwJBAJ2tTJH6L9+w5WgDI5pOBpd843MPRLsMbffdCi4i\nzDL2R/vh8tsBmw3GRYcm/9VLLNzdHtgHVwDn9ttRWYNUspA=\n-----END RSA PRIVATE KEY-----\n",
  "server_key": "",
  "ca_keys": {},
  "uid": "tester"
}

const ws = new ClientConnectionHandler('ws://127.0.0.1:8080/', key);
ws.generate_chat_keys(["tester", "jonathan"], "test_chat").then(() => {
  /*ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
  ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")*/
  return ws.send_message("The dragons cant beat the mouse. because tha mouse is to strong", "test_chat")
}).then(() => 
  ws.get_message("test_chat", 0, 1024)
).then((v) => 
  console.log(v)
)