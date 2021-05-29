<?php
    $this->setTitle('Trang Cá Nhân');
    $this->addJSFiles(['profile']);
    $this->addOnloadBodyFuncs(
        ["getUser" => []]
    );
?>

<div class="container">
    <div class="row">
        <div class="col-md-12">
            <div class="card">
                <div class="card-body">
                    <div class="row">
                        <div class="col-md-12">
                            <h4>Thông tin của bạn</h4>
                            <hr>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-md-12">
                            <?php Form::begin('/editProfile', 'get'); ?>
                                <div class="form-group row">
                                    <label for="name" class="col-4 col-form-label">Họ Tên</label> 
                                    <div class="col-8" id='nameHolder'></div>
                                </div>
                                <div class="form-group row">
                                    <label for="email" class="col-4 col-form-label">Email</label> 
                                    <div class="col-8" id='emailHolder'></div>
                                </div>
                                <div class="form-group row">
                                    <label for="email" class="col-4 col-form-label">Gia nhập từ</label> 
                                    <div class="col-8" id='createdAtHolder'></div>
                                </div>
                                <div class="form-group row">
                                    <div class="offset-4 col-8">
                                    <button type="submit" class="btn btn-primary">Thay đổi</button>
                                    </div>
                                </div>
                            <?php Form::end(); ?>
                        </div>
                    </div>
                    
                </div>
            </div>
        </div>
    </div>
</div>