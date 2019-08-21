function showLogs(room_number){
    var id = "id_" + room_number;
    var classID = "collapse_" + room_number;
    console.log(id);
    document.getElementById(id).style.visibility = "visible";
    document.getElementsByClassName(classID)[0].style.visibility = "visible";
}

function viewRoomLog(room_number){
    console.log(room_number);
    var url = "/roomlogs/"+room_number
    console.log(url);
    document.location.href= url;
}
