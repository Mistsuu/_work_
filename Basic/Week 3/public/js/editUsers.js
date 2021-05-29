function getUpdateUserResult(result, args)
{
    result = JSON.parse(result);
    
    // Modify HTML to restore...
    user      = result.user;
    container = args['container'];
    container.getElementsByClassName('id')[0]       .innerHTML = user.id;
    container.getElementsByClassName('firstName')[0].innerHTML = user.firstName;
    container.getElementsByClassName('lastName')[0] .innerHTML = user.lastName;
    container.getElementsByClassName('email')[0]    .innerHTML = user.email;
    container.getElementsByClassName('password')[0] .innerHTML = (user.password.length > 20 ? user.password.substr(0, 20) + '...' : user.password);
    container.getElementsByClassName('isAdmin')[0]  .innerHTML = (user.privilege == 1 ? 'Có' : 'Không');

    // Display fail message if you want to...
    if (result.status == 'failure') {
        console.log(result.errors);
    }
}

function getRemoveUserResult(result, args)
{
    result = JSON.parse(result);
    if (result.status == 'success') {
        args['container'].remove();
    } else {
        console.log(result.errors);
    }
}

function getAddUserResult(result, args)
{
    // Get data...
    result    = JSON.parse(result);
    user      = result.user;
    container = args['container'];

    // Display fail message if you want to...
    if (result.status == 'failure') {
        console.log(result.errors);
        return;
    }

    // Modify HTML to restore...
    container.getElementsByClassName('id')[0]       .innerHTML = user.id;
    container.getElementsByClassName('firstName')[0].innerHTML = user.firstName;
    container.getElementsByClassName('lastName')[0] .innerHTML = user.lastName;
    container.getElementsByClassName('email')[0]    .innerHTML = user.email;
    container.getElementsByClassName('password')[0] .innerHTML = (user.password.length > 20 ? user.password.substr(0, 20) + '...' : user.password);
    container.getElementsByClassName('isAdmin')[0]  .innerHTML = (user.privilege == 1 ? 'Có' : 'Không');

    // Return the ol' two buttons
    container.getElementsByClassName('edit-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('remove-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('removeadd-button')[0].setAttribute('hidden', true);
    container.getElementsByClassName('add-button')[0].setAttribute('hidden', true);
}

function updateUser(newUser, container)
{
    httpPostAsync('/modifyUser', {'newUser' : JSON.stringify(newUser)}, getUpdateUserResult, {'container' : container});
}

function addUser(newUser, container)
{
    httpPostAsync('/addUser', {'newUser' : JSON.stringify(newUser)}, getAddUserResult, {'container' : container});
}

function removeUser(container)
{
    var userID = container.getElementsByClassName('id')[0].innerHTML;
    httpPostAsync('/removeUser', {'userID' : userID}, getRemoveUserResult, {'container' : container});
}

function toggleOffEditUserForm(container)
{
    container.getElementsByClassName('edit-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('save-button')[0].setAttribute('hidden', true);

    // Store input field values
    var user = {};
    user.firstName = container.getElementsByClassName('input-firstName')[0]['value'];
    user.lastName  = container.getElementsByClassName('input-lastName') [0]['value'];
    user.email     = container.getElementsByClassName('input-email')    [0]['value'];    
    user.password  = container.getElementsByClassName('input-password') [0]['value'];
    user.privilege = container.getElementsByClassName('input-isAdmin')  [0].checked;
    user.id        = container.getElementsByClassName('id')             [0].innerHTML;

    // Change user
    updateUser(user, container);
}

function toggleOffAddUserForm(container)
{
    var newUser = {};
    
    newUser.firstName = container.getElementsByClassName('input-firstName')[0]['value'];
    newUser.lastName  = container.getElementsByClassName('input-lastName') [0]['value'];
    newUser.email     = container.getElementsByClassName('input-email')    [0]['value'];    
    newUser.password  = container.getElementsByClassName('input-password') [0]['value'];
    newUser.privilege = container.getElementsByClassName('input-isAdmin')  [0].checked;

    // Add user
    addUser(newUser, container);
}

function toggleOnAddUserForm()
{
    // Get template
    var userContainerTemplate = document
        .getElementById('userListHolder')
        .getElementsByClassName('userHolder')
        [0];

    // Clone container
    var cloneContainer = userContainerTemplate.cloneNode(true);
    cloneContainer.removeAttribute('hidden');

    // Change every field into an input field
    cloneContainer.getElementsByClassName('id')[0]       .innerHTML = '#';
    cloneContainer.getElementsByClassName('firstName')[0].innerHTML = `<input type="text"     class="form-control     input-firstName">`;
    cloneContainer.getElementsByClassName('lastName')[0] .innerHTML = `<input type="text"     class="form-control     input-lastName" >`;
    cloneContainer.getElementsByClassName('email')[0]    .innerHTML = `<input type="text"     class="form-control     input-email"    >`;
    cloneContainer.getElementsByClassName('password')[0] .innerHTML = `<input type="text"     class="form-control     input-password" >`;
    cloneContainer.getElementsByClassName('isAdmin')[0]  .innerHTML = `<input type="checkbox" class="form-check-input input-isAdmin"  >`;

    // Replace the two buttons
    cloneContainer.getElementsByClassName('add-button')      [0].removeAttribute('hidden');
    cloneContainer.getElementsByClassName('removeadd-button')[0].removeAttribute('hidden');
    cloneContainer.getElementsByClassName('edit-button')     [0].setAttribute('hidden', true);
    cloneContainer.getElementsByClassName('remove-button')   [0].setAttribute('hidden', true);

    // Add to the end of the user list
    document.getElementById('userListHolder').appendChild(cloneContainer);
}

function toggleOnEditUserForm(container)
{
    // Replace the two buttons
    container.getElementsByClassName('save-button')[0].removeAttribute('hidden');
    container.getElementsByClassName('edit-button')[0].setAttribute('hidden', true);

    // Get isAdmin attribute
    isAdmin = container.getElementsByClassName('isAdmin')[0].innerHTML == 'Có';

    // Change every field into an input field
    container.getElementsByClassName('firstName')[0].innerHTML = `<input type="text"     class="form-control     input-firstName" value="` + container.getElementsByClassName('firstName')[0].innerHTML + `">`;
    container.getElementsByClassName('lastName')[0] .innerHTML = `<input type="text"     class="form-control     input-lastName"  value="` + container.getElementsByClassName('lastName')[0] .innerHTML + `">`;
    container.getElementsByClassName('email')[0]    .innerHTML = `<input type="text"     class="form-control     input-email"     value="` + container.getElementsByClassName('email')[0]    .innerHTML + `">`;
    container.getElementsByClassName('password')[0] .innerHTML = `<input type="text"     class="form-control     input-password"  value="">`;
    container.getElementsByClassName('isAdmin')[0]  .innerHTML = `<input type="checkbox" class="form-check-input input-isAdmin">`;
    container.getElementsByClassName('input-isAdmin')[0].checked = isAdmin;
}

function updateUserList(usersData)
{
    // Loop through users
    var users = [];
    users = JSON.parse(usersData);
    users.forEach((user, index) => {
        // Get template
        var userContainerTemplate = document
            .getElementById('userListHolder')
            .getElementsByClassName('userHolder')
            [0];
        
        // Clone container
        var cloneContainer = userContainerTemplate.cloneNode(true);
        cloneContainer.removeAttribute('hidden');

        // Set values
        cloneContainer.getElementsByClassName('id')[0]       .innerHTML = user.id;
        cloneContainer.getElementsByClassName('firstName')[0].innerHTML = user.firstName;
        cloneContainer.getElementsByClassName('lastName')[0] .innerHTML = user.lastName;
        cloneContainer.getElementsByClassName('email')[0]    .innerHTML = user.email;
        cloneContainer.getElementsByClassName('password')[0] .innerHTML = (user.password.length > 20 ? user.password.substr(0, 20) + '...' : user.password);
        cloneContainer.getElementsByClassName('isAdmin')[0]  .innerHTML = (user.privilege == 1 ? 'Có' : 'Không');

        // Add to the end of the user list
        document.getElementById('userListHolder').appendChild(cloneContainer);
    });
}

function removeContainer(container)
{
    container.remove();
}

function getAllUsers()
{
    httpPostAsync('/getAllUsers', {}, updateUserList);
}