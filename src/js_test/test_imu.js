// This is how we connect to the creator. IP and port.
// The IP is the IP I'm using and you need to edit it.
// By default, MALOS has its 0MQ ports open to the world.

// Every device is identified by a base port. Then the mapping works
// as follows:
// BasePort     => Configuration port. Used to config the device.
// BasePort + 1 => Keepalive port. Send pings to this port.
// BasePort + 2 => Error port. Receive errros from device.
// BasePort + 3 => Data port. Receive data from device.

var creator_ip = process.env.CREATOR_IP || '127.0.0.1'
var creator_imu_base_port = 20013

var zmq = require('zmq')

// Import MATRIX Proto messages
var matrix_io = require('matrix-protos').matrix_io


// ********** Start error management.
var errorSocket = zmq.socket('sub')
errorSocket.connect('tcp://' + creator_ip + ':' + (creator_imu_base_port + 2))
errorSocket.subscribe('')
errorSocket.on('message', (error_message) => {
  console.log('Message received: IMU error: ', error_message.toString('utf8'))
});
// ********** End error management.


// ********** Start configuration.
var configSocket = zmq.socket('push')
configSocket.connect('tcp://' + creator_ip + ':' + creator_imu_base_port)

// Now prepare valid configuration and send it.
var config = matrix_io.malos.v1.driver.DriverConfig.create({
  delayBetweenUpdates: 2.0,  // 2 seconds between updates
  timeoutAfterLastPing: 6.0  // Stop sending updates 6 seconds after pings.
})

configSocket.send(matrix_io.malos.v1.driver.DriverConfig.encode(config).finish())
// ********** End configuration.

// ********** Start updates - Here is where they are received.
var updateSocket = zmq.socket('sub')
updateSocket.connect('tcp://' + creator_ip + ':' + (creator_imu_base_port + 3))
updateSocket.subscribe('')
updateSocket.on('message', (imu_buffer) => {
  var imuData = matrix_io.malos.v1.sense.Imu.decode(imu_buffer)
  console.log(imuData)
});
// ********** End updates

// ********** Ping the driver
var pingSocket = zmq.socket('push')
pingSocket.connect('tcp://' + creator_ip + ':' + (creator_imu_base_port + 1))
console.log("Sending pings every 5 seconds");
pingSocket.send(''); // Ping the first time.
setInterval(() => {
  pingSocket.send('');
}, 5000);
// ********** Ping the driver ends
