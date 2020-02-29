module.exports = function(RED) {
  function cToF(config) {
    RED.nodes.createNode(this, config);
    var node = this;
    node.on("input", function(msg) {
      var tempc = msg.payload;
      var tempf = (tempc * 9) / 5 + 32;
      msg.payload = tempf;
      node.send(msg);
    });
  }
  RED.nodes.registerType("celcius-to-farenheit", cToF);
};
