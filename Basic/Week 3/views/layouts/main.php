<!doctype html>
<html lang="en">
    <head>
        <!-- Required meta tags -->
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

        <!-- Bootstrap CSS/JS/JQuery -->
        <script src="js/jquery/jquery-3.3.1.slim.min.js"></script>
        <!-- <script src="js/bootstrap/bootstrap.bundle.min.js"></script> -->
        <link rel="stylesheet" href="css/bootstrap/bootstrap.min.css">
        <link rel="stylesheet" href="css/bootstrap/font-awesome.min.css">
        
        <!-- My customized CSS code -->
        <?php echo $this->loadCssFilesAsHTML(); ?>

        <!-- My customized JS code -->
        <?php echo $this->loadJSFilesAsHTML(); ?>
        
        <!-- My hard-coded JS code -->
        <script src='js/ajaxhandler.js'></script>
        <script src='js/navbar.js'></script>
        <script src='js/flash.js'></script>

        <!-- Hard-coded CSS file for the layout -->
        <link rel="stylesheet" href="css/sticky-footer.css">
        
        <!-- Renders title -->
        <title><?php echo $this->getTitle(); ?></title>
    </head>
    <body onload="changeNavBarButtons(); displayFlashes();<?php echo $this->loadOnloadBodyFuncsAsHTML(); ?>">
        
        <!-- For navigation bar only... -->
        <nav class="navbar navbar-expand-lg navbar-light bg-light">
            <a class="navbar-brand" href="/">SuShop</a>
            <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarSupportedContent" aria-controls="navbarSupportedContent" aria-expanded="false" aria-label="Toggle navigation">
                <span class="navbar-toggler-icon"></span>
            </button>

            <div class="collapse navbar-collapse" id="navbarSupportedContent">
                <!-- Home/Contact -->
                <ul class="navbar-nav mr-auto">
                    <li class="nav-item active">
                        <a class="nav-link" href="/">Trang chủ <span class="sr-only">(current)</span></a>
                    </li>
                    <li class="nav-item active">
                        <a class="nav-link" href="/contact">Liên hệ</a>
                    </li>
                </ul>

                <!-- Login/Register or Profile/Logout -->
                <div id="twoButtons">
                </div>
                
            </div>
        </nav>

        <!-- Display alerts -->
        <br>
        <div class="container" id="alertSuccessHolder"></div>
        <div class="container" id="alertDangerHolder"></div>

        <!-- NOTE: ALL THE CODE FOR THE VIEW LAYS INSIDE HERE! -->
        <br>
        <div class="container">
            {{content}}
        </div>
        <br><br>
        
        <!-- Footer -->
        <footer class="footer">
            <div class="container">
                <span class="text-muted">Website nguyên gốc được sao chép lại bởi Mistsu, 5/2021.</span>
            </div>
        </footer>

        <!-- Optional JavaScript -->
        <!-- jQuery first, then Popper.js, then Bootstrap JS -->
        <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js" integrity="sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN" crossorigin="anonymous"></script>
        <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js" integrity="sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q" crossorigin="anonymous"></script>
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js" integrity="sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl" crossorigin="anonymous"></script>
    </body>
</html>

