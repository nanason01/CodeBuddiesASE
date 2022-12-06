function logError(request, status, error){
    console.log("Error")
    console.log(request)
    console.log(status)
    console.log(error)
    
}

var apikey = "";
var clientid = "";
var refresh_token = "";

$(document).ready(function(){
    console.log("Hello World")
    $("#submit1").click(function(){
        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/getcredentials",
            success: function(result){
                console.log(result)
                $("#1clientid").html("Client ID: " + result['client_id'])
                $("#1apikey").html("API key: " + result['api_key'])
                $("#1refreshtoken").html("Refresh Token: " + result['refresh_token'])

                apikey = result['api_key'];
                clientid = result['client_id']
                refresh_token = result['refresh_token']
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })

    $("#submit2").click(function(){
        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/refreshcredentials",
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + refresh_token);
            },
            success: function(result){
                console.log(result)
                $("#2clientid").html("Client ID: " + result['client_id'])
                $("#2apikey").html("API key: " + result['api_key'])
                $("#2refreshtoken").html("Refresh Token: " + result['refresh_token'])

                apikey = result['api_key'];
                clientid = result['client_id']
                refresh_token = result['refresh_token']
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })

    $("#submit3").click(function(){
        let trade = {
            "timestamp": $("#3timestamp").val(),
            "sold_currency": $("#3sc").val(),
            "bought_currency": $("#3bc").val(),
            "sold_amount": $("#3sa").val(),
            "bought_amount": $("#3ba").val()
        }
        console.log(trade)

        $.ajax({
            type: "POST",
            url: "http://0.0.0.0:18420/trade",
            dataType: "json",
            data: JSON.stringify(trade),
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#3resp").html("Success! HTTP " + xhr.status + " " + textstatus)
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })

    $("#submit4").click(function(){
        let exchangekeys = {
            "exchange": $("#4exchange").val(),
            "secretkey": $("#4secretkey").val(),
            "readkey": $("#4readkey").val()
        }
        console.log(exchangekeys)
        $.ajax({
            type: "POST",
            url: "http://0.0.0.0:18420/exchangekey",
            dataType: "json",
            data: JSON.stringify(exchangekeys),
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#4resp").html("Success! HTTP " + xhr.status + " " + textstatus)
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })

    $("#submit5").click(function(){
        let exchange = {
            "exchange": $("#5exchange").val()
        }
        $.ajax({
            type: "POST",
            url: "http://0.0.0.0:18420/removekey",
            dataType: "json",
            data: JSON.stringify(exchange),
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#5resp").html("Success! HTTP " + xhr.status + " " + textstatus)
            },
            error: function(request, status, error){
                console.log("Error")
                console.log(request)
                console.log(status)
                console.log(error)
                
            }
        })
    })

    $("#submit6").click(function(){
        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/get_annotated_trades",
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                
                //$("#6resp").html("200 OK. No records found.")
                $("#6resp").html("Success! HTTP " + xhr.status + " " + result["status"])
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

    $("#submit7").click(function(){
        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/year_end_stats",
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#lrp").html("lt_realized_pnl: " + result["lt_realized_pnl"])
                $("#srp").html("st_realized_pnl: " + result["st_realized_pnl"])
                $("#ap").html("actual_pnl: " + result["actual_pnl"])
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

    $("#submit8").click(function(){
        let trade = {
            "timestamp": $("#8timestamp").val(),
            "sold_currency": $("#8sc").val(),
            "bought_currency": $("#8bc").val(),
            "sold_amount": $("#8sa").val(),
            "bought_amount": $("#8ba").val()
        }
        console.log(trade)
        $.ajax({
            type: "POST",
            url: "http://0.0.0.0:18420/trade_pnl",
            dataType: "json",
            data: JSON.stringify(trade),
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#tradepnl").html("The pnl for this trade is " + result["pnl"])
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

    $("#submit9").click(function(){
        $.ajax({
            type: "GET",
            url: "http://0.0.0.0:18420/portfolio_pnl",
            beforeSend: function (xhr) {
                xhr.setRequestHeader('Authorization', 'Bearer ' + apikey);
            },
            success: function(result, textstatus, xhr){
                $("#portfoliopnl").html("The portfolio pnl is " + result["pnl"])
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