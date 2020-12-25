package main

import (
	"bytes"
	"compress/zlib"
	"fmt"
	"io"
	"net"
)

func handleConnection(conn net.Conn) {
	buf := make([]byte, 0)
	tmp := make([]byte, 1024*1024)
	total := 0
	for {
		n, err := conn.Read(tmp)
		total += n
		if err != nil {
			if err == io.EOF {
				goto handleEOF
			}
			fmt.Printf("read error %+v", err)
		}
		buf = append(buf, tmp[:n]...)
	}

handleEOF:
	fmt.Printf("totally read: %d\n", total)
	buffer := bytes.NewBuffer(buf[:total])

	if r, err := zlib.NewReader(buffer); err != nil {
		fmt.Printf("uncompress,%+v", err)
	} else {
		r.Close()
		fmt.Printf("done :%d\n", buffer.Len())
	}
}

func main() {
	if ln, err := net.Listen("tcp", ":2020"); err != nil {
		fmt.Printf("%+v", err)
	} else {
		for {
			conn, err := ln.Accept()
			if err != nil {
				fmt.Printf("%+v", err)
			}
			go handleConnection(conn)
		}
	}
}
