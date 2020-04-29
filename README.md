# TLS Server Client

## Introduce
This repo will show you an easy TLS Server and Client communication.

## Run
```
$ make init // set own ca private key and ca information

$ make runserver

// then open another terminal
$ make runclient
```

## Client command

```
Remote File copy
        list     Show file list
        getf     Get file from server
        delf     Delete file from server
     
Remote shell
        cmd      Send command to server
     
Message communication
        echo     Send message and recieve echo from server
     
Other
        help     Show command list
```

### Remote File copy
Provide files in `storage` folder
#### `list`

show file list of `storage` 

#### `getf <filename>`

download `storage/<filename>` to `downloads/<filename>`

#### `delf <filename>`

delete `storage/<filename>`

### Remote shell

#### `cmd <command>`

execute `<command>` on remote
BUT can't execute interact program, just can execute commad like `ls`, `cat`, either use cd directory.

### Message communication

#### `echo <message>`

server will echo `<message>` sent from client