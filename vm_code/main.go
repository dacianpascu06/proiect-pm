package main

import (
	"fmt"
	"net/http"
	"strings"
)

var database = map[string]int{
	"2AF185E1": 1,
	"473B7925": 0,
}

func handlePm(w http.ResponseWriter, r *http.Request) {
	parameters := r.URL.RawQuery
	parts := strings.Split(parameters, "=")

	if len(parts) < 2 {
		http.Error(w, "missing uid", http.StatusBadRequest)
		return
	}
	uid := strings.ToUpper(parts[1])
	allowed, ok := database[uid]
	fmt.Printf("Received id %s\n", uid)

	response := fmt.Sprintf("verdict=%d", 0)

	if !ok {
		w.Write([]byte(response))
		println("unknown uid")
		return
	}
	if allowed == 1 {
		response = fmt.Sprintf("verdict=%d", 1)
	}
	w.Write([]byte(response))
}

func main() {
	println("Starting server")
	http.HandleFunc("/pm", handlePm)
	http.ListenAndServe(":4000", nil)
}
