fs = global.fs || require('fs')

console.log 'fs', Object.keys(fs)

writeBuffer = if global.Buffer then new Buffer 1024 else createSimpleBuffer 1024

if true
	for i in [0...1000]
		fs.open './asyncFiles/afw' + i, 'w', (err, fd)->
			#console.log 'open cb', err, fd
			fs.write fd, writeBuffer, 0, writeBuffer.length, 0, (err, bytesWritten, buf)->
				#console.log 'written', err, bytesWritten
				fs.close fd, (err)->
					#console.log 'closed', err
if false
	fs.open = (path, flags, callback)-> callback null, fs.openSync path, flags
	fs.write = (fd, buffer, offset, length, position, callback)-> callback null, fs.writeSync(fd, buffer, offset, length, position), buffer
	fs.close = (fd, callback)-> callback fs.closeSync fd

	testPath = 'z:/asyncFiles/af'
	if false
		fd = fs.openSync testPath + 'w', 'w'
		console.log 'fd', fd
		numBytes = fs.writeSync fd, writeBuffer, 0, writeBuffer.length, 0
		console.log 'writeBuffer', numBytes
		console.log 'closeSync', fs.closeSync fd
	else
		fs.open testPath + 'w', 'w', (err, fd)->
			console.log 'open'
			fs.write fd, writeBuffer, 0, writeBuffer.length, 0, (err, bytesWritten, buf)->
				console.log 'written'
				fs.close fd, ->
					console.log 'closed'