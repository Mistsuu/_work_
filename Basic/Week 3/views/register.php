<?php
    $this->setTitle('Đăng kí');
?>

<h1>Đăng kí</h1>

<br>
<?php Form::begin('', "post"); ?>
    <div class="row">
        <div class="col">
            <?php echo (new InputField($model, 'firstName'))->setType('text') ?>
        </div>
        <div class="col">
            <?php echo (new InputField($model, 'lastName')) ->setType('text') ?>
        </div>
    </div>
    
    <?php echo (new InputField($model, 'email'))          ->setType('email') ?>
    <?php echo (new InputField($model, 'password'))       ->setType('password') ?>
    <?php echo (new InputField($model, 'passwordConfirm'))->setType('password') ?>

    <button type="submit" class="btn btn-primary">Gửi</button>
<?php Form::end(); ?>