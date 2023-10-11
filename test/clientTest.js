const { ClientConnectionHandler } = require('../lib');

const key = {
  "sign_key": "3l2daxxHqgPpZ5rPCZfsXKEZ+HlT3POAtlFITpsC0P8=",
  "data_key": "-----BEGIN RSA PRIVATE KEY-----\nMIIEpAIBAAKCAQEAs0GiTYqcw7NFkWEZXUMGGh8pDc2HUwYx+60jDa0Af6R1VGu+\nKZiyA6bXKPDUmCoRftvhOuGaOq8d8FL28ybEW/xBOsIAtJtIN4ZoD0ME0VvTCMm1\nVz8wVqYrI0vFftlZoslKqRwKXe2cPIEBzkB6fJrwsAkUczxowzc7N9DHqWd4CESJ\ndpzQ4ijr2oi0YhytWKNpjTwi9WapS2xNUbYMRquxP2QVAFvKg4nO+jtZkxMFMAel\nhuxo2AQ8/cBAUw8uDI+jfop4OUux21xhUvkHgG0i1OObsAulV8LfqTFlE+tnlzKf\nzfyUS2XTxXRiuiUMLCs9nlSgLBxkqpfJbPBJIwIDAQABAoIBAH2mOhBEYxJCbTc0\nasUWoAZbZTmJoZuusgaKHBIP/FN8gcnFJCygHde1cQf/l+V5R342vi+5x5Pu//tf\nDD7wI5+euFJpfnkah4p0KijYf14JEWoNiZE4d7elDoPVLwB1dgFHO5BCWXnxfhxj\nCVL7m2AQcznRg4YrB/wekOHU9L9m2ryHSTjdP27XGE068OaTTl+5mzYga34UiXmY\nVZJAHO9Nn+YmCd5G4/IaYU93c/lFBFXskZbYa0errPTuwbdhwWeTiyfymABpYaFD\na21k2QKQIwrqwxEGNKxyiyEB4bTqpUm65Ay+ehiWOXGGv01V4RXkWinG410UP7jD\nNWj+mqECgYEA9AMmn4ZnXUdoI6hn1sqprYvpiOjX+iYwiTWo8cSyk4RQfORXhwVZ\nez8m59lmHmNZ4SUqw9HNO9zCw1NsHUGb3C55eMp0gmhEOHswT2xiw8d7AkypnBPD\nh0/i5DsPoOZ+gGsI6NW3oBsVDK96kCdkrA8AtMSAJ83D2DL8OWxCOskCgYEAvBAS\nnRyhyKTTWumtDWthAVImAWlJ+D7xyKujG3jriSsnZMuKXUuesLesUjyorL5S5QJ3\nGqRKxqpplZH3XqaoW0zg18XgFVdm+rQ21SRue/GDhZIrXVV01aYFra6Xcnzpr7jF\n5bGPRF/WheLlKlff5HeueWQ0wrV7tUmSXdX4bosCgYEA3FE//UdYasN/hrqWTzyb\nKYQrbx91anoy1qBpkynbZxBPsARn9xRgb8Rcd3g2GPfxfjSVwUrcONGh1MSs2FmV\nxZ78tJBv1BMajbjErOGa51Rkf3YCqla586Gwpguhl9ClpYN+/uOAsYHiZprGWgQH\nhMbjB1Rotge2ba3TSHGz9ukCgYEAmnRU5UZ7tS9yCsu6fjf/nrcaUbRotQL1Twtw\nVYLVZ33Sx9zHUeJmfdsMIyqAmgdiS4Xfzji61TlmqBoO4WTdBsc4Lt7sMHrtL+BM\n3HckapUroMsw5GHoKARHy6KF7dibkN+fYQuYj95qcOGttz4euyR0XmynTHcDGBxJ\notm2RQsCgYBVksEFbucu8uNil42rIED5XeS52tLxAM8RVpFX5bw0ZGAj91gLMtsI\nDeqTqSuKgtyuQIo/XdLVGO2mfZFjpc02qB+zL7sAnEfjXURbcE6W/5S/hxf3vIUV\nKf+SF0jZtbj0pCkGveeQcC/7RXDkEbrCTCh2ek52JG0UGTiKasYUZw==\n-----END RSA PRIVATE KEY-----",
  "server_key": "",
  "ca_keys": {},
  "uid": "d"
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