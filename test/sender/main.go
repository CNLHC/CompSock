package main

import (
	"bytes"
	"compress/zlib"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"net"
	"os"
	"time"
)

type Info struct {
	PayloadFile string
	TotalRead   int64
	TotalWrite  int64
	Elapsed     time.Duration
}

func (c Info) Show() {
	KBps := float64(c.TotalWrite) / float64(float64(c.Elapsed.Microseconds())/1000000.0) / 1024
	MBps := KBps / 1024
	Mbps := MBps * 8
	Ratio := 100 * float64(c.TotalWrite) / float64(c.TotalRead)
	fmt.Printf("total read %d Bytes\n", c.TotalRead)
	fmt.Printf("total write %d Bytes\n", c.TotalWrite)
	fmt.Printf("Ratio %f\n", Ratio)
	fmt.Printf("speed %f Mbps/s\n", Mbps)
	fmt.Printf("speed(equ.) %f Mbps/s\n", Mbps/Ratio)
}

func main() {
	var (
		err  error
		conn net.Conn
		info Info
	)

	send_file := flag.String("i", "./payload", "sending payload")
	flag.Parse()
	info.PayloadFile = *send_file

	if send_file == nil {
		fmt.Errorf("invalid payload file %v", 1)
		os.Exit(1)
	}

	if conn, err = net.Dial("tcp", "localhost:2020"); err != nil {
		fmt.Printf("conn err %+v", err)
		os.Exit(1)
	}
	defer conn.Close()

	var buffer bytes.Buffer

	payload, err := ioutil.ReadFile(*send_file)
	info.TotalRead = int64(len(payload))
	if err != nil {
		fmt.Errorf("can not read payload %+v", err)
		os.Exit(1)
	}

	w := zlib.NewWriter(&buffer)
	w.Write(payload)
	w.Close()

	//perform IO
	start := time.Now()
	info.TotalWrite = int64(buffer.Len())
	io.Copy(conn, &buffer)
	info.Elapsed = time.Since(start)
	info.Show()

}
