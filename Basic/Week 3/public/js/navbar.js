function changeNavBarHTML(userStatus)
{
    var container = document.getElementById('twoButtons');
    // Status = 'user' -> Profile/Logout
    if (userStatus != 'guest') {
        // Change HTML if user
        if (userStatus == 'user')
            container.innerHTML = `
                <ul class="navbar-nav ml-auto">
                    <li class="nav-item active">
                        <a class="nav-link" id='username' href="/profile"></a>
                    </li>
                    <li class="nav-item active">
                        <a class="nav-link" href="/viewCart">Giỏ hàng</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="/logout">Đăng xuất</a>
                    </li>
                </ul>
            `;

        // Change HTML if admin
        if (userStatus == 'admin')
            container.innerHTML = `
                <ul class="navbar-nav ml-auto">
                    <li class="nav-item active">
                        <a class="nav-link" id='username' href="/profile"></a>
                    </li>
                    <li class="nav-item active">
                        <a class="nav-link" href="/viewCart">Giỏ hàng</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="/editUsers">Quản lí người dùng</a>
                    </li>
                    <li class="nav-item">
                        <a class="nav-link" href="/editStuffs">Quản lí mặt hàng</a>
                    </li>
                    <li class="nav-item active">
                        <a class="nav-link" href="/logout">Đăng xuất</a>
                    </li>
                </ul>
            `;

        // Change username holder
        httpGetAsync('/getUsername', {}, changeUsernameHolder, {'userStatus' : userStatus});
    }
    // Status = 'guest' -> Login/Register
    else
        container.innerHTML = `
            <ul class="navbar-nav ml-auto">
                <li class="nav-item active">
                    <a class="nav-link" href="/login">Đăng nhập <span class="sr-only">(current)</span></a>
                </li>
                <li class="nav-item">
                    <a class="nav-link" href="/register">Đăng kí</a>
                </li>
            </ul>
        `;
}

function changeUsernameHolder(username, args)
{
    var container = document.getElementById('username');
    container.innerHTML = 'Chào, ' + username;
    if (args['userStatus'] == 'admin') 
        container.innerHTML += ' (Admin)';
}

function changeNavBarButtons()
{
    httpGetAsync('/getUserStatus', {}, changeNavBarHTML);
}