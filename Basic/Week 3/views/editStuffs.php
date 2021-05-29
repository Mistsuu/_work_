<?php 
    $this->setTitle('Quản lí sản phẩm');
    $this->addCSSFiles(['editStuffs']); 
    $this->addJSFiles(['editStuffs']); 
    $this->addOnloadBodyFuncs([
        'getAllStuffs' => []
    ]);
?>

<!-- Demo header-->
<section class="pb-5 header text-center">
    <div class="container py-5 text-white">
        <header class="py-5">
            <h1 class="display-4">Quản lí mặt hàng</h1>
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
                                        <th scope="col">Tên</th>
                                        <th scope="col">Giá</th>
                                        <th scope="col">Mô tả</th>
                                        <th scope="col">Đơn vị</th>
                                        <th scope="col">Chỉnh sửa</th>
                                    </tr>
                                </thead>
                                <tbody id="stuffListHolder">
                                    <tr class="stuffHolder" hidden>
                                        <th scope="col" class="id"> </th>
                                        <td scope="col" class="name"></td>
                                        <td scope="col" class="price"></td>
                                        <td scope="col" class="description"></td>
                                        <td scope="col" class="currency"></td>
                                        <td scope="col" class="modify">
                                            <ul class="list-inline m-0">
                                                <li class="list-inline-item add-button" hidden>
                                                    <button class="btn btn-success btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Thêm" onclick="toggleOffAddStuffForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-plus-square"></i></button>
                                                </li>
                                                <li class="list-inline-item save-button" hidden>
                                                    <button class="btn btn-success btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Lưu" onclick="toggleOffEditStuffForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-save"></i></button>
                                                </li>
                                                <li class="list-inline-item edit-button">
                                                    <button class="btn btn-info btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Sửa" onclick="toggleOnEditStuffForm(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-edit"></i></button>
                                                </li>
                                                <li class="list-inline-item remove-button">
                                                    <button class="btn btn-danger btn-sm circle-0" type="button" data-toggle="tooltip" data-placement="top" title="Xóa" onclick="removeStuff(this.parentNode.parentNode.parentNode.parentNode);"><i class="fa fa-trash"></i></button>
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
                    <button class="btn btn-block btn-success" onclick="toggleOnAddStuffForm();">Thêm mặt hàng</button>
                </div>
            </div>
            
        </div>
    </div>
</section>