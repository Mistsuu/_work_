<?php
    $this->setTitle('Trang Cá Nhân');
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
                            <?php Form::begin('', "post"); ?>
                                <?php echo (new InputField($model, 'firstName'))->setType('text') ?>
                                <?php echo (new InputField($model, 'lastName'))->setType('text') ?>
                                <?php echo (new InputField($model, 'email'))->setType('email') ?>
                                <?php echo (new InputField($model, 'newPassword'))->setType('password') ?> 
                                <button type="submit" class="btn btn-primary">Thay đổi</button>
                            <?php Form::end(); ?>
                        </div>
                    </div>
                    
                </div>
            </div>
        </div>
    </div>
</div>