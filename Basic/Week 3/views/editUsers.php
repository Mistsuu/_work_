<?php 
    $this->setTitle('Quản lí người dùng');
    $this->addCSSFiles(['editUsers']); 
    $this->addJSFiles(['editUsers']); 
    $this->addOnloadBodyFuncs([
        'getAllUsers' => []
    ]);
?>

<!-- Demo header-->
<section class="pb-5 header text-center">
    <div class="container py-5 text-white">
        <header class="py-5">
            <h1 class="display-4">Quản lí người dùng</h1>
            <p class="font-italic mb-1">Hãy làm một admin tốt chứ đừng làm một assmean!</p>
        </header>


        <div class="row">
            <div class="col-lg-12 mx-auto">
                <div class="card border-0 shadow">
                    <div class="card-body p-5">

                        <!-- Responsive table -->
                        <div class="table-responsive">
                            <table class="table m-0">
                                <thead>
                                    <tr>
                                        <th scope="col">ID</th>
                                        <th scope="col">Ảnh đại diện</th>
                                        <th scope="col">Họ</th>
                                        <th scope="col">Tên</th>
                                        <th scope="col">Mật khẩu</th>
                                        <th scope="col">Email</th>
                                        <th scope="col">Admin</th>
                                        <th scope="col">Chỉnh sửa</th>
                                    </tr>
                                </thead>
                                <tbody id="userListHolder">
                                    <tr class="userHolder" hidden>
                                        <th scope="col" class="id"> </th>
                                        <td scope="col" class="img"> <img src="https://dummyimage.com/50x50/55595c/fff" /></td>
                                        <td scope="col" class="firstName"></td>
                                        <td scope="col" class="lastName"></td>
                                        <td scope="col" class="password"></td>
                                        <td scope="col" class="email"></td>
                                        <td scope="col" class="isAdmin"></td>
                                        <td scope="col" class="modify">
                                            <ul class="list-inline m-0">
                                                <li class="list-inline-item add-button" hidden>
                                                    <button class="btn btn-success btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Thêm" onclick="toggleOffAddUserForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-plus-square"></i></button>
                                                </li>
                                                <li class="list-inline-item save-button" hidden>
                                                    <button class="btn btn-success btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Lưu" onclick="toggleOffEditUserForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-save"></i></button>
                                                </li>
                                                <li class="list-inline-item edit-button">
                                                    <button class="btn btn-info btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Sửa" onclick="toggleOnEditUserForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-edit"></i></button>
                                                </li>
                                                <li class="list-inline-item remove-button">
                                                    <button class="btn btn-danger btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Xóa" onclick="removeUser(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-trash"></i></button>
                                                </li>
                                                <li class="list-inline-item removeadd-button" hidden>
                                                    <button class="btn btn-danger btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Xóa" onclick="removeContainer(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-trash"></i></button>
                                                </li>
                                            </ul>
                                        </td>
                                    </tr>
                                </tbody>
                            </table>

                        </div>
                    </div>
                    <button class="btn btn-block btn-success" onclick="toggleOnAddUserForm();">Thêm người dùng</button>
                </div>
            </div>
            
        </div>
    </div>
</section>