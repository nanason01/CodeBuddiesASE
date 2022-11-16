$(document).ready(function(){
    console.log("Hello World")

    $("#submit1").click(function(){
        
        let apikey = "abcdef";
        let clientid = "ghijk";
        let ret = "lmnop";

        $("#1clientid").html(clientid);
        $("#1apikey").html(apikey)
        $("#1refreshtoken").html(ret)

        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/getcredentials",
            success: function(result){
                console.log(result)
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })
})