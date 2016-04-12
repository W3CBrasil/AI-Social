<!DOCTYPE html>
<html lang="pt-br">
<head>
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="initial-scale = 1.0,maximum-scale = 1.0" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />    
  <title>Dados na Web</title>
  <link href="/favicon.ico" type="image/x-icon" rel="icon" />
  <link href="/favicon.ico" type="image/x-icon" rel="shortcut icon" />    

  <link rel="stylesheet" href="assets/bootstrap/css/bootstrap.min.css">
  <link rel="stylesheet" href="assets/css/font-awesome.css">
  <link rel="stylesheet" href="assets/css/theme.css">
</head>
<body>
  <header>
    <div class="first-line">
      <div class="container">
        <div class="logos-top pull-right">
          <a href="http://nic.br"><img src="assets/images/logo-nicbr-topo.png" alt="Logo NIC.Br"></a>
          <a href="http://cgi.br"><img src="assets/images/logo-cgibr-topo.png" alt="Logo CGI.Br"></a>
        </div>
      </div>
    </div>
  </header>

  <main>
    <div class="banner img-resposive">
      <div class="row col-xs-12">
        <div class="col-xs-2 visible-lg visible-md">
          <a href="/"><img src="/assets/images/logo-dados.png" alt="Inteligência Analítica aplicada ao Processo de Particapação socil na web" class="img-responsive"></a>
        </div>
        <div class="col-xs-10">
          <div class="box-title-banner">
            <p class="title-banner">Inteligência Analítica aplicada ao </p>
            <p class="second-title-banner">Processo de Particapação Socil na Web</p>
          </div>
        </div>
      </div>

      <div class="container-fluid">
       <div class="third-line top-bar">
        <nav class="navbar navbar-inverse col-xs-12">
          <div class="container-fluid">
            <div class="container row">
              <div class="col-xs-11 col-md-offset-1">
                <div class="navbar-header">
                  <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#menu">
                    <span class="sr-only">Toggle Navigation</span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                  </button>
                </div>
                <div class="collapse navbar-collapse nav-banner" id="menu">
                  <ul class="nav navbar-nav">
                    <li><a href="/"><span class="glyphicon glyphicon-home"></span>
                      <span class="sr-only">Home</span>
                    </a></li>
                    <li><a href="#">Documentos</a></li>
                    <li><a href="#">Relatórios</a></li>
                    <li><a href="#">Dados</a></li>
                  </ul>
                </div>
              </div>
            </div>
          </div>
        </nav>
      </div>
    </div>

  </div>
  <div class="col-xs-12 first-container">
    <div class="col-md-2 col-xs-12 bhoechie-tab-menu">
      <div class="list-group">
        <?php include("inc/sobre.php"); ?>
        <?php include("inc/classificacao.php"); ?>
        <?php include("inc/correlacoes.php"); ?>
        <?php include("inc/topicos.php"); ?>
      </div>
    </div> <!-- /bhoechie-tab-menu -->

    <div class="col-md-10 col-xs-12 bhoechie-tab">
      <!-- Tab 1 -->
      <?php include("inc/tabs/tab1.php"); ?>

      <!-- Tab 2 -->
      
      <?php include("inc/tabs/tab2.php"); ?>
      

      <!-- Tab 3 -->
      <?php include("inc/tabs/tab3.php"); ?>

      <!-- Tab 4 --> 

      <?php include("inc/tabs/tab4.php"); ?>

    </div>
  </div>

  <div class="col-xs-12 second-container">
    <div class="col-md-3 col-xs-12">
      <p class="title-desc">Outras visualizações</p>

      <div class="box-desc">
        <a href="" title="Voronoi Circle"><img src="assets/images/voronoi-circles.png" class="img-responsive" alt="Alt da imagem"></a>
      </div>

      <div class="box-desc">
        <a href="" title="Voroblobinoids"><img src="assets/images/voroblobinoids.png" class="img-responsive" alt="Alt da imagem"></a>
      </div>

      <div class="box-desc">
        <a href="" title="Color Mesh"><img src="assets/images/color-mesh.png" class="img-responsive" alt="Alt da imagem"></a>
      </div>

      <div class="box-desc">
        <a href="" title="Canvas Bar Chart"><img src="assets/images/canvas-bar-chart.png" class="img-responsive" alt="Alt da imagem"></a>
      </div>

      <div class="box-desc">
        <a href="" title="Cubehelix Color Picker"><img src="assets/images/cubehelix-color-picker.png" class="img-responsive" alt="Alt da imagem"></a>
      </div>


    </div>
    <div class="col-md-6 col-xs-12 mini-graphic">
     <p class="title-desc"> De onde veio a idéia?</p>
     <p class="title-author">Por Yaso</p>
     <div class="img-graphic">
      <img src="assets/images/img-exemple.png" class="img-responsive" alt="Alt da imagem">
    </div>
    <div class="row container-mini-graphic">
      <div class="col-xs-12">
        <p class="title-desc">O projeto</p>
        <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut sit amet diam a erat ornare mollis id non arcu. Nam in risus purus. Etiam aliquam egestas bibendum. Praesent et erat quis enim gravida efficitur. Nullam posuere porttitor venenatis. Suspendisse id pulvinar mauris, et vehicula risus. Cras ut turpis pulvinar, feugiat odio sed, gravida arcu. Quisque ut varius odio. Vestibulum eu dignissim risus.</p>
      </div>

      <div class="col-xs-12">
        <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
        <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
        <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
      </div>


      <div class="col-xs-12">
       <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
       <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
       <div class="col-sm-4 col-xs-12 img-mini-graphic"><img src="assets/images/img-exemple-02.png" class="img-responsive" alt="Alt da imagem"></div>
     </div>
   </div>
 </div>
 <div class="col-md-3 col-xs-12 description">
  <div class="box-desc">
    <p class="title-desc">O projeto</p>
    <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut sit amet diam a erat ornare mollis id non arcu. Nam in risus purus. Etiam aliquam egestas bibendum. Praesent et erat quis enim gravida efficitur. Nullam posuere porttitor venenatis. Suspendisse id pulvinar mauris, et vehicula risus. Cras ut turpis pulvinar, feugiat odio sed, gravida arcu. Quisque ut varius odio. Vestibulum eu dignissim risus.</p>
  </div>

  <div class="box-desc">
    <p class="title-desc">O projeto</p>
    <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut sit amet diam a erat ornare mollis id non arcu. Nam in risus purus. Etiam aliquam egestas bibendum. Praesent et erat quis enim gravida efficitur. Nullam posuere porttitor venenatis. Suspendisse id pulvinar mauris, et vehicula risus. Cras ut turpis pulvinar, feugiat odio sed, gravida arcu. Quisque ut varius odio. Vestibulum eu dignissim risus.</p>
  </div>

  <div class="box-desc">
    <p class="title-desc">O projeto</p>
    <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut sit amet diam a erat ornare mollis id non arcu. Nam in risus purus. Etiam aliquam egestas bibendum. Praesent et erat quis enim gravida efficitur. Nullam posuere porttitor venenatis. Suspendisse id pulvinar mauris, et vehicula risus. Cras ut turpis pulvinar, feugiat odio sed, gravida arcu. Quisque ut varius odio. Vestibulum eu dignissim risus.</p>
  </div>

  <div class="box-desc">
    <p class="title-desc">O projeto</p>
    <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut sit amet diam a erat ornare mollis id non arcu. Nam in risus purus. Etiam aliquam egestas bibendum. Praesent et erat quis enim gravida efficitur. Nullam posuere porttitor venenatis. Suspendisse id pulvinar mauris, et vehicula risus. Cras ut turpis pulvinar, feugiat odio sed, gravida arcu. Quisque ut varius odio. Vestibulum eu dignissim risus.</p>
  </div>

  <div class="box-desc">
    <p class="title-desc">Parceiros</p>
    <div class="soon-partner">
      <a href="#"><img src="assets/images/inweb.png" alt="Logo Parceiros - Inweb"></a>
    </div>
    <div class="soon-partner">
      <a href="#"><img src="assets/images/inweb.png" alt="Logo Parceiros - Inweb 2"></a>
    </div>
  </div>


  <div class="box-desc">
    <p class="title-desc time-involved">Time envolvido</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
    <p><strong>Lorem Ipsum, </strong>turpis pulvina</p>
  </div>
</div>
</div> 
</main>
<div class="teste-footer">
  <ul class="links-footer">
    <li class="teste-link-cgi"><a href="http://cgi.br" title="Visite o site do CGI.br"></a></li>
    <li class="teste-link-nic"><a href="http://nic.br" title="Visite o site do NIC.br"></a></li>
    <li class="teste-link-registro"><a href="http://registro.br" title="Visite o site do Registro.br"></a></li>
    <li class="teste-link-cert"><a href="http://cert.br" title="Visite o site do Cert.br"></a></li>
    <li class="teste-link-cetic"><a href="http://cetic.br" title="Visite o site do Cetic.br"></a></li>
    <li class="teste-link-ceptro"><a href="http://ceptro.br" title="Visite o site do Ceptro.br"></a></li>          
    <li class="teste-link-ptt"><a href="http://ptt.br/" title="Visite o site do PTT"></a></li>
    <li class="teste-link-ceweb"><a href="http://www.ceweb.br" title="Visite o site do Ceweb"></a></li>
    <li class="teste-link-w3c"><a href="http://www.w3c.br/Home/WebHome" title="Visite o site do W3C"></a></li>
    <li class="teste-link-trabalhe"><a href="/vagas/"></a></li>
    <li class="teste-link-quem"><a href="/quem-somos/"></a></li>
  </ul>

  <figure>
    <div class="container">   
      <img src="assets/images/footer.png" alt="Logos do CGI.br, NIC.br, Registro.br, Cert.br, Ceptro.br, Cetic.br e W3C" class="img-responsive">
    </div>
  </figure>
  <div class="menu-footer">
    <div class="container">
      <div class="col-lg-12 col-md-12 col-sm-12 text-left license">
        <div class="row">
          <div class="col-lg-2 col-md-2 col-sm-3">
            <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Licença Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a>
          </div>
          <div class="col-lg-10 col-md-10 col-sm-9">Artigos e informações publicados no site da <a href="http://egi.nic.br/">Escola de Governança da Internet no Brasil</a> estão licenciados com uma Licença <a href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons - Atribuição-CompartilhaIgual 4.0 Internacional</a>, a menos que condições e/ou autorizações adicionais específicas estejam claramente explicitas. <br />
          </div>
        </div>
      </div>
    </div>
  </div>
</div>

<script src="assets/js/vendor/jquery-1.11.1.min.js"></script>
<script src="assets/bootstrap/js/bootstrap.min.js"></script>
<script src="assets/js/main.js"></script>

</body>
</html>