<?php
    $this->setTitle('Liên hệ');
?>

<h1>Liên hệ với chúng tôi</h1>
<h5>Lưu ý: Có khoảng tầm 100% khả năng chúng tôi sẽ không quan tâm.</h5>

<br><br>
<?php Form::begin('', 'post') ?>
    <?php echo new InputField($model, 'subject'); ?>
    <?php echo new InputField($model, 'email'); ?>
    <?php echo new TextareaField($model, 'body') ?>
    <button type="submit" class="btn btn-primary">Gửi</button>
<?php Form::end(); ?>