function updateUserInfo(userInfo)
{
    userInfo = JSON.parse(userInfo);
    console.log(userInfo);

    document.getElementById('nameHolder').innerHTML      = userInfo.firstName + ' ' + userInfo.lastName;
    document.getElementById('emailHolder').innerHTML     = userInfo.email;
    document.getElementById('createdAtHolder').innerHTML = userInfo.created_at;
}

function getUser()
{
    httpGetAsync('/getUser', {}, updateUserInfo);
}