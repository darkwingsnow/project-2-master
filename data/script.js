window.onload = () => {

    /**
     * This function send a request to server to get current temperature 
     * then update the content of the tempture and background color
     * 
     *  */
    function getTmp() {
        
        // get the body element
        let box = document.body

        // fetch temperautre Data
        fetch("temp").then(
            response => response.json()
        ).then(
            data => {
                // Update temperaure
                document.getElementById("temp").innerHTML = data
                // while the temper getting higer, red color take more space on the screen
                // assuming the temperature change range from 10 to 50, 
                // temp lower than 10 will all blue, higher than 10 will be red, other than that will be gradient
                let pencent = ((data - 10) / 40) * 100
                console.log(data, pencent);
                box.setAttribute("style", "background:linear-gradient(90deg,#F8242D " + pencent + "%,  #0880F2  100%) no-repeat;")
            }
        )
    }
    getTmp();
    // Fetch sensor id and show on the screem
    fetch("sensorId").then(
        response => response.json()
    ).then(
        data => {
            document.getElementById("sensorId").innerHTML = "Sensor ID:" + data
        }
    )
    setInterval(() => {
        getTmp()
    }, 1000)
}