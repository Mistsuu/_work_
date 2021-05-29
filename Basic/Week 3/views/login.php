<?php
    $this->setTitle('Đăng nhập');
?>

<h1>Đăng nhập</h1>

<br>
<?php Form::begin('', "post"); ?>

    <?php echo (new InputField($model, 'email')   )->setType('email'); ?>
    <?php echo (new InputField($model, 'password'))->setType('password'); ?>

    <button type="submit" class="btn btn-primary">Gửi</button>
    
<?php Form::end(); ?>