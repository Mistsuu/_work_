function changeFlashSuccess(flashMessage)
{
    if (flashMessage != '')
        document.getElementById('alertSuccessHolder').innerHTML = `
            <div class="alert alert-success">
                ` + flashMessage + `
            </div>
        `;
}

function changeFlashDanger(flashMessage)
{
    if (flashMessage != '')
        document.getElementById('alertDangerHolder').innerHTML = `
            <div class="alert alert-danger">
                ` + flashMessage + `
            </div>
        `;
}

function displayFlashes()
{
    httpGetAsync('/getFlashMessage', {'flashName' : "success"}, changeFlashSuccess);
    httpGetAsync('/getFlashMessage', {'flashName' : "danger"},  changeFlashDanger);
}