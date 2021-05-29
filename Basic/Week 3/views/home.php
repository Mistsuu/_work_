<?php 
    $this->setTitle('Trang chủ');
    $this->addJSFiles(['home', 'addToCart']);
    $this->addOnloadBodyFuncs(
        ['getAllStuffs' => []]
    );
?>

<main role="main">

    <section class="jumbotron text-center">
    <div class="container">
        <h1 class="jumbotron-heading">Chào mừng đến với SuShop!</h1>
        <p class="lead text-muted">Shop này bán những mặt hàng vô dụng nhất quả đất với giá cả siêu phải chăng.</p>
        <!-- <p>
        <a href="#" class="btn btn-primary my-2">Main call to action</a>
        <a href="#" class="btn btn-secondary my-2">Secondary action</a>
        </p> -->
    </div>
    </section>

    <div class="album py-5 bg-light">
    <div class="container">

        <!-- Modify -->
        <div class="row" id="stuffsHolder">
        </div>

    </div>
    </div>

</main>