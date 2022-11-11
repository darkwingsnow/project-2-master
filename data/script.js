window.onload = () => {

    let temp = 20.00

    let tempText = document.getElementById("temp")
    /**
     *  add eventListener for radio button
     */
    var onChange = () => {
        let checked = document.querySelector('input[type=radio]:checked')
        tempText.setAttribute("style", "visibility:hidden")
        // make the text disappre, so that we know the text is updating
        setTimeout(() => {
            tempText.setAttribute("style", "visibility:visible")
        }, 50)
        console.log(checked.value);
        if (checked.value == "F") {
            tempText.innerHTML = (temp * 1.8 + 32).toFixed(2) + "&#8457;"
        } else {
            tempText.innerHTML = temp.toFixed(2) + "&#8451;"
        }


    }

    document.querySelectorAll('input[type=radio]').forEach(input => input.addEventListener('change', onChange))

    /**
     * This function send a request to server to get current temperature 
     * then return the tempcure
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
                // while the temper getting higer, red color take more space on the screen
                // assuming the temperature change range from 10 to 50, 
                // temp lower than 10 will all blue, higher than 10 will be red, other than that will be gradient
                let pencent = ((data - 10) / 40) * 100
                box.setAttribute("style", "background:linear-gradient(90deg,#F8242D " + pencent + "%,  #0880F2  100%) no-repeat;")
                temp = data
            })

    }

    tempText.innerHTML = temp + "&#8451;"

    // Fetch sensor id and show on the screem
    fetch("sensorId").then(
        response => response.json()
    ).then(
        data => {
            document.getElementById("sensorId").innerHTML = "Sensor ID:" + data
        }
    )
    setInterval(() => {
        // display the text
        getTmp()
        onChange()
    }, 5000)
}