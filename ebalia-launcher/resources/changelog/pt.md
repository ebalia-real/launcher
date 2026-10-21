# 1.2.3 (2026-10-20)

- As Versões perdidas são baixadas da própria publicação da EBALIA, e os arquivos delas não trazem mais registros e relatórios de erro que sobravam de outros launchers. As que você já instalou continuam funcionando; a próxima que você instalar baixa o arquivo dela mais uma vez.

# 1.2.2 (2026-10-18)

- Explorar mods busca enquanto você digita (mods, pacotes de recursos e shaders, e os modpacks ao criar uma instância): os resultados chegam assim que você faz uma pausa, sem apertar Enter, que continua buscando na hora. Se uma busca não puder ser feita (sem conexão, sem instância), a lista avisa em vez de uma janela.
- Buscar em Explorar mods não trava mais: as imagens são baixadas e passam pelo cache em uma thread própria, a lista é redesenhada uma vez por busca, e a página completa, a galeria e a capa de um mod carregam quando a escolha fica parada e você para de digitar.
- As colaborações da EBALIA (como Neighbor.jar) não aparecem mais como próprias: um mod novo da EBALIA com outros autores entra nas colaborações, com o crédito desses autores, no site e no launcher. O Início mostra só os mods próprios.
- A janela EBALIA Mods é nova: os mods próprios e as colaborações separados, como no site, em cartões como os do Início com a imagem, os downloads, quem fez, a descrição no seu idioma e botões pequenos de CurseForge e Modrinth; em cima, o total de downloads; o Patreon com o que oferece. O convite do Patreon tem o mesmo estilo.
- Bedrock: Jogar fica ao lado da versão em vez de atravessar o cartão, as ações dividem uma linha e a ajuda é uma linha de links.

# 1.2.1 (2026-10-18)

- O aviso de atualização mostra as novidades: as notas de cada versão desde a sua até a oferecida, no idioma do launcher, com Atualizar e reiniciar ou Mais tarde abaixo.
- Mudanças: uma seção da barra lateral (e a versão que aparece embaixo dela) lista cada versão com a data, da mais nova para a mais antiga, e mostra o que mudou na que você escolher, no idioma do launcher; a busca mantém as versões que mencionam uma palavra e a destaca. A sua também aparece marcada.
- Os campos de busca (o filtro do registro, Explorar mods, instâncias e versões perdidas) mantêm a lupa centralizada entre a borda e o texto.
- Ícones novos na barra lateral: pixel art no estilo do jogo, desenhada para a EBALIA: um bloco de grama para o Início, uma picareta para Instâncias, uma bússola, um fantasma para as versões perdidas, um baú para Meus packs, um jogador para Skins, uma folha de papel para Notícias, dois jogadores para Comunidade, um livro para o guia, um bloco de bedrock, um relógio para Mudanças e uma bigorna para Ajustes.
- O Início mostra o site oficial da EBALIA, ebalia.gitgud.site: o cartão de prévia no seu idioma, o que ele tem e um botão para abri-lo.
- Corrigido: ao trocar o fundo do Início com as setas, a imagem exibida pulava para a posição da próxima enquanto ela carregava, e parecia uma faixa atravessando o banner.
- A picareta de Instâncias é a do Minetest Game (CC BY-SA 3.0).
- As Configurações abrem a pasta onde o launcher está (onde foi instalado, ou onde você colocou a cópia portable) e a mostram.
- Desinstalar, nas Configurações: o launcher fecha e se remove (com o desinstalador da instalação no Windows; de uma pasta portable ou descompactada, só os arquivos que ela lista, e o resto fica; o arquivo AppImage ou o app do macOS) e, se você marcar, também seus dados. Cópias instaladas pelo sistema (Nix, pacotes) são removidas com o gerenciador de pacotes.

# 1.2.0 (2026-10-15)

- Usa cerca de metade da memória: as imagens carregam no tamanho em que são exibidas e não em tamanho cheio (os fundos do Início não são mais todos lidos antecipadamente, as imagens das notícias e os cartões das versões perdidas são pequenos, os ícones da janela de Mods ficam com 64 pixels), os caches de imagens são menores e minimizar o launcher (como ao jogar) libera as imagens guardadas para depois. No Linux, a memória liberada volta para o sistema.
- As versões perdidas instalam só o próprio jogo. Alguns pacotes do arquivo foram guardados junto com o launcher que os executava (UltimMC, BetaCraft) e com outras versões; o EBALIA inicia o jogo por conta própria, então isso não é mais instalado: NSSS ocupa 4 MB em vez de 700, e Alpha 1.0.16 e Minecraft a1.1.1 deixam de lado o UltimMC e o BetaCraft. As versões instaladas antes são organizadas na próxima vez que iniciam e mantêm seus mundos, opções e registro.
- As versões perdidas mostram suas próprias capturas, feitas em cada versão: a imagem grande, uma faixa para escolher outra e um visualizador em tamanho cheio. Cada uma conta o que é (uma descrição mais longa em cada idioma), o que o jogo mostra como versão, se é uma recriação ou foi resgatada pela EBALIA, quem a fez, seu tamanho instalada e para baixar, e com o que funciona. Os cartões da lista mostram as mesmas capturas.
- Corrigido: Minecraft 1.Li02 fechava com um erro ao criar um mundo (sua tela Create New World nunca preparava o jogador). Agora o EBALIA o inicia com essa tela corrigida, sem alterar os arquivos do jogo; mundos de Sobrevivência e Hardcore funcionam.
- O registro ao vivo faz parte da página da instância, abaixo das capturas e dos servidores, e acompanha o jogo enquanto ele roda. Tela cheia dá a ele a página inteira (o botão de registro no topo o abre assim); Escape ou o link de voltar retornam.

# 1.1.9 (2026-10-13)

- Mais rápido: ler os ícones dos mods, as imagens e descrições dos pacotes, encontrar pacotes no Modrinth e as cabeças das skins rodam em algumas threads próprias em segundo plano e são lembrados durante a sessão. Antes, abrir a janela de Mods ocupava todas as threads do processador por alguns segundos e tudo o que você fazia em seguida (instalar, buscar, remover, abrir outra janela) esperava atrás; agora começa na hora. Reabrir uma janela, remover um mod ou desativá-lo não lê mais todos os arquivos de novo.
- Fechar o launcher o encerra na hora: downloads e consultas ainda em andamento param em vez de manter o processo vivo por minutos numa conexão lenta.
- As imagens da internet são decodificadas fora da thread da janela, o cache de imagens baixadas tem um limite e a capa da página do mod é redimensionada uma vez por tamanho; as imagens ficam nítidas em telas com escala.
- Explorar mods diz o que a instância já tem: mods, pacotes de recursos e shaders instalados aparecem como «Já instalado» na lista, o botão Instalar também diz isso e instalar vários os deixa de fora. Os pacotes são registrados ao serem instalados; os adicionados à mão ou antes são reconhecidos pelo Modrinth. Um pacote ou mod removido da sua pasta pode ser instalado de novo.
- As imagens dos mods ficam nítidas: o visualizador da galeria e a capa da página do mod mostram as imagens como foram enviadas em vez das prévias de 350 pixels do Modrinth (a prévia aparece enquanto a original baixa), e a faixa do CurseForge usa suas prévias pequenas.
- As listas suspensas (Mods, Pacotes de recursos, Shaders e qualquer outra escolha) abrem embaixo da sua caixa com uma única borda; no Windows, partes da lista eram desenhadas fora dela.
- Os cartões de Pacotes de recursos e Shaders têm um só link, como Mods: a janela deles tem o botão de download.
- Shaders, e pacotes de recursos sem imagem, mostram o ícone do seu projeto no Modrinth (encontrado pelo arquivo e guardado em cache). Os botões Usar este shader e Ativar mantêm o mesmo espaço entre ícone e texto que os outros.
- Registro ao vivo: a janela do registro mostra a saída do jogo enquanto ela é escrita, acompanha o final a menos que você role para cima, colore erros de vermelho e avisos de amarelo, filtra linhas, copia tudo e não bloqueia mais o launcher atrás dela. As versões perdidas também têm um botão de registro.
- Versões perdidas: Desinstalar fica junto de Jogar e Reinstalar sempre que a versão está instalada.
- Os downloads mostram a porcentagem (e o tamanho na dica): as versões perdidas e o Java delas, e a instalação das instâncias ao lado do que está sendo feito.
- O Bedrock procura o jogo uma vez, e de novo só depois de algo feito pelo launcher (instalar, atualizar, desinstalar, abrir a Store ou o Minecraft Linux) ou com Verificar novamente, em vez de toda vez que a página abre.

# 1.1.8 (2026-10-13)

- Corrigido: os botões Instalar e Abrir página da página do mod fechavam o launcher em todos os sistemas (e os botões «Instalar <mod>» e «Baixar o driver» do resumo de erros também podiam fazer isso).
- Explorar mods tem um único botão Instalar: ele instala cada item escolhido (Ctrl ou Shift para escolher vários) e diz quantos; os mods que não têm versão para a instância são listados enquanto o resto instala.
- Pacotes de recursos e shaders do CurseForge e do Modrinth: escolha Mods, Pacotes de recursos ou Shaders em Explorar mods, ou toque em Baixar nesses cartões de uma instância. Os shaders oferecem Iris (Fabric, Quilt, NeoForge) ou Oculus (Forge) quando a instância não tem nenhum dos dois. Instâncias vanilla podem receber pacotes de recursos.
- A página do mod mostra a galeria dele: miniaturas, e as imagens grandes com ◀ ▶ e as setas do teclado.
- O ícone do mod fica centralizado e não encosta mais na borda do painel quando o mod não tem imagem de capa.
- O menu do ícone da instância sempre mostra as mesmas imagens que as instâncias (ele ficava com as incluídas depois que as texturas do jogo eram lidas).
- As imagens das contas mostram a cabeça da skin que o jogador usa (Microsoft) ou recebe por padrão (sem premium) em vez da imagem do Xbox; uma imagem que você escolheu continua tendo prioridade. A lista de Skins mostra a cabeça de cada skin.
- Pacotes de recursos, shaders e mundos abrem a própria janela a partir da página da instância (clique em qualquer parte do cartão), como os mods: cada pacote com sua imagem e descrição, ativar ou desativar pacotes de recursos e escolher o shader que o jogo carrega, adicionar ZIPs, remover (guardados em removed-… dentro da instância), abrir a pasta e baixar mais. Mundos: abrir a pasta de um mundo, exportá-lo como ZIP, importar um mundo em ZIP. A janela de mods também abre a pasta dela.
- A página Skins mostra a skin num modelo 3D: arraste para girar, clique duas vezes para vê-la de frente de novo, Frente / costas dá meia-volta. A visão plana está a um botão.
- No GNOME e em outros desktops Wayland que deixam a barra de título para o aplicativo, o EBALIA desenha a sua: clique duas vezes para maximizar ou restaurar, arraste para mover a janela e redimensione pelas bordas. Windows, macOS, X11, KDE e os demais mantêm a barra de título do sistema, onde o clique duplo já faz isso. EBALIA_TITLE_BAR=system ou custom força uma ou outra.
- As skins podem ser excluídas da biblioteca (Excluir skin, ou a tecla Delete); o PNG de onde foram importadas é mantido.
- Guia e tutorial: os passos do guia são cartões lado a lado, todos à vista sem rolar, e as perguntas frequentes têm sua própria aba.
- Silence fala no tour interativo: o texto aparece letra por letra com pequenos sons de voz, um toque marca cada passo e uma melodia curta o final. O botão de alto-falante do tour o silencia (fica lembrado). Os sons são gerados pelo próprio launcher e tocam pelo sistema (PlaySound no Windows, sons do sistema no macOS, PulseAudio ou PipeWire no Linux): sem arquivos de som e sem bibliotecas novas.
- Traduções: a nota sobre a skin padrão dos perfis sem premium estava em inglês em sete idiomas; o tour agora chama as Versões perdidas como a barra lateral em cada idioma; as dicas da barra de título acompanham a mudança de idioma.

# 1.1.7 (2026-10-12)

- O Bedrock no Linux e no macOS passa pelo modo de perfil do próprio Minecraft Linux, que verifica a compra no Google Play e carrega seu módulo de compatibilidade: a versão que você escolhe no EBALIA inicia como pelo botão Jogar dele. A importação de APK e o início direto do jogo foram removidos (eles pulavam a verificação de compra).
- O Bedrock lista as versões baixadas para escolher, mantém o Minecraft Linux atualizado (Flathub no Linux; o app verificado para Apple Silicon no macOS 15+) e diz por que um jogo fechou (compra não confirmada, versão incompatível, componentes faltando).
- Bedrock no Windows: Minecraft e Minecraft Preview para escolher, um botão do Game Pass (app do Xbox), uma verificação dos Gaming Services, um aviso em máquinas virtuais sem placa de vídeo e uma desinstalação que primeiro copia os mundos de onde as versões atuais os guardam.
- Ajuda para entrar no Xbox (Remote Connect no Linux e no macOS).
- Criar um perfil sem premium antes de baixar qualquer versão do jogo podia travar o launcher quando ele tinha sido aberto por um terminal (um caminho vazio do arquivo do jogo fazia o leitor de arquivos esperar entrada do teclado).

# 1.1.6 (2026-10-12)

- Explorar mods: a página do mod lê as descrições como o Modrinth (Markdown com HTML dentro, pelo md4c), então texto, tabelas, títulos e imagens aparecem; as imagens se encaixam enquanto baixam sem a página pular, e a capa mostra a imagem inteira em vez de um recorte ampliado.
- O clique duplo instala mesmo com a lista ainda carregando (começa assim que termina); o painel da página fica sempre visível, então a lista não se mexe mais embaixo do mouse.
- Tema personalizado: uma prévia ao vivo de um pedaço do launcher nas cores escolhidas.
- Início em tela cheia: o banner cresce com a janela em vez de encolher até virar uma faixa.

# 1.1.5 (2026-10-11)

- Bedrock sem outro launcher: no Windows, o EBALIA detecta o Minecraft para Windows e o inicia diretamente (ou abre sua página na Microsoft Store); no Linux, instala o ambiente do Minecraft Linux, importa o APK do Minecraft que você comprou e inicia o jogo por conta própria. Novo botão Desinstalar (no Windows, os mundos são copiados antes; no Linux, os mundos ficam a menos que você escolha apagá-los).
- Explorar mods: uma prévia do mod selecionado (imagem, descrição, downloads), clique duplo para instalar, só mods para a versão da instância e sem barra de carregamento quando mais resultados carregam no fim da lista.
- Dois mods que precisam de versões diferentes da mesma dependência (muitas vezes o Fabric API) não interrompem mais a instalação: a mais nova é mantida.
- Tudo traduzido: os textos mais novos, as perguntas frequentes, o tour e as mensagens de instalação, download, Java, skins, Bedrock e atualização, nos dez idiomas.
- A roda do mouse nunca muda uma lista suspensa (como a do idioma), nem depois de clicar nela.

# 1.1.4 (2026-10-11)

- Explorar mods carrega os mods populares assim que a aba abre (e quando a instância ou o provedor muda); Enter busca mesmo com outra tarefa em andamento, e rolar até o fim carrega mais resultados.
- Tema personalizado: escolha suas próprias cores de destaque e de fundo em Configurações → Tema → Personalizado.
- A roda do mouse rola a página: as listas suspensas (como a do idioma) só mudam depois que você clica nelas.
- O launcher não fica mais cinza atrás de uma caixa de diálogo aberta; ele só ignora os cliques até a caixa fechar.
- Mudar o tema é instantâneo: só as cores são aplicadas de novo, em vez de reconstruir todas as páginas.
- Os mods mostram seus próprios ícones na janela de mods, para todos os carregadores (Fabric, Quilt, Forge, NeoForge, o antigo mcmod.info); mods sem ícone recebem o do Modrinth.
- Skins mostra a skin que você usa: a skin atual da conta Microsoft ou, nos perfis sem premium, a skin padrão que o Minecraft dá a eles.
- As versões perdidas também usam texturas do Minecraft (bloco de grama, abóbora de Halloween para as versões de terror, bancada de trabalho).
- Nova instância: Personalizado mostra o bloco de grama do Minecraft e Importar, um ícone de importação limpo.
- Servidores na página da instância: um botão Editar, os nomes completos dos servidores (sem os códigos de cor do Minecraft) e os títulos Servidores e Capturas de tela ficam no topo quando as listas estão vazias.

# 1.1.3 (2026-10-08)

- Temas: EBALIA (verde), Deep Dark (turquesa), Nether (vermelho), End (roxo), Oceano (azul) e Cerejeira (rosa), em Configurações, aplicados na hora.
- Perguntas frequentes em Guia e tutorial: importar packs, travamentos, comprar o Minecraft, mods, Java, máquinas virtuais, onde ficam os mundos, atualizações, uso portátil, ícones e imagens, versões perdidas e avisos do antivírus.
- As capas de EBALIA Mods no Início usam arte escura (Nether, Deep Dark, mina abandonada).

# 1.1.2 (2026-10-08)

- Travamentos do driver gráfico e a falta de OpenGL mostram sua placa de vídeo (dedicada ou integrada ao processador) e abrem o driver dela: a página exata do driver para placas NVIDIA GeForce e AMD Radeon RX, as páginas da Intel para Arc e gráficos da 7ª à 10ª geração, e a ferramenta de detecção automática do fabricante (NVIDIA App, AMD Auto-Detect, Intel Driver & Support Assistant) quando a página exata não é encontrada. Notebooks e PCs de marca também recebem a página de suporte do fabricante (Dell, HP, Lenovo, ASUS, Acer, MSI, GIGABYTE, Samsung).
- Bedrock: o launcher instala o runtime do Microsoft Visual C++ de que o gerenciador do Bedrock precisa (ele parava com «You need VC++ Runtime 14.14…»), e depois que o gerenciador está instalado um botão Jogar o inicia.
- Os ícones das instâncias usam as próprias texturas do Minecraft (picareta de diamante, TNT, bancada de trabalho, creeper…), lidas do jogo que o launcher baixou.
- Escolha sua própria foto de perfil no menu da conta (e volte para a da Microsoft quando quiser).
- O Início mostra EBALIA Mods com capas, acima das notícias.
- Uma página do Patreon mais limpa: cartão de assinatura, publicações organizadas com datas legíveis e «Ver mais», em Comunidade e Notícias.
- Removida a configuração do endereço do serviço do Patreon; nenhuma faixa vazia embaixo das páginas; «Suporte e comunidade» mostra o seu «e».

# 1.1.1 (2026-10-07)

- Máquinas virtuais e computadores sem driver gráfico: quando o Minecraft fecha porque não há OpenGL, o launcher oferece iniciá-lo com gráficos por software (OpenGL desenhado pelo processador; Mesa no Windows, o Mesa do sistema no Linux). Também dá para ativar nas configurações da instância. O Windows o usa para o Minecraft 1.13 e posteriores.
- Downloads do jogo mais rápidos: as conexões com os servidores da Mojang são reutilizadas e as bibliotecas baixam em paralelo. Um Minecraft 1.21.1 novo (3.900 arquivos, 850 MB) passou de 143 para 28 segundos no nosso teste.
- Fundos de instância: escolha qualquer imagem do launcher ou uma do seu computador, pela página da instância ou pelas configurações dela.
- O ícone da instância também pode ser trocado pelas configurações da instância.
- Quando o Minecraft fecha com um erro, um resumo diz o que falhou, com um botão para corrigir: um mod faltando (instalado do Modrinth para a versão e o carregador da instância), um travamento do driver gráfico (link para o driver da AMD, NVIDIA ou Intel, ou gráficos por software), falta de OpenGL, o Java errado ou pouca memória. O registro completo está a um clique.
- Importar pastas .minecraft: a do Minecraft Launcher oficial e as pastas de versão dos launchers que usam o mesmo formato (TLauncher, SKLauncher…), compactadas ou não. A versão do Minecraft e o carregador vêm do perfil da versão ou, quando não há, dos próprios mods. Essas pastas também aparecem na lista de instâncias encontradas no computador, prontas para importar. Os arquivos de contas nunca são copiados.
- As pastas das instâncias levam o nome da instância em vez de um número aleatório; renomear uma instância renomeia a pasta dela. As instâncias de versões anteriores são renomeadas ao iniciar e continuam funcionando (uma pasta em uso mantém o nome antigo).
- Cole instâncias de outros launchers direto na pasta de instâncias (Prism Launcher, MultiMC, app do CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App, TLauncher ou pastas de versão do .minecraft, arquivos .zip e .mrpack): o launcher as transforma em instâncias com a versão do Minecraft e o carregador, inclusive enquanto está aberto. O original vai para a lixeira de instâncias só quando isso deu certo.
- Toda importação verifica se os mods combinam com a versão do Minecraft e o carregador do pack e diz quais não combinam.

# 1.1.0 (2026-10-05)

- O login da Microsoft está ativado com o ID público de aplicativo da EBALIA (`microsoft-client-id.txt`).
- As contas Microsoft agora mostram a foto de perfil (a imagem de jogador do Xbox da conta; o rosto da skin do Minecraft quando não há nenhuma) na barra lateral, no cartão do jogador do Início, no menu da conta e nas Configurações. As imagens ficam em `mc/avatars`.
- Silence, a mascote da EBALIA, acompanha o tour interativo e a página Guia e tutorial, animada: flutua, respira, pisca, os olhos brilham, balança uma faca, dá um pulinho a cada passo e se inclina para o controle destacado.
- Modo portátil: um `portable.txt` ao lado do launcher (ao lado do AppImage ou do bundle `.app`) guarda instâncias, contas e configurações numa pasta `ebalia-data` ao lado.
- Silence agora tem uma folha de sprites em pixel art com sete poses (parada, feliz, surpresa, piscando, apontando, comemorando, pensando), cada uma com quadros para falar e piscar; cada passo do tour usa sua própria pose.
- Novos ícones de instância de 16×16 desenhados à mão. Qualquer imagem do seu computador pode ser usada como ícone de instância, ao criar a instância ou depois pela página dela.
- A página da instância mostra suas capturas (clique para abrir) e sua lista de servidores multijogador, onde dá para adicionar, copiar e remover servidores; os campos desconhecidos do `servers.dat` são mantidos.
- Rodada de tradução: o tour interativo, as páginas do Patreon e de criações, a desinstalação das Versões perdidas, as mensagens de login da Microsoft e o atualizador do launcher estão traduzidos para os dez idiomas.
- Site: downloads do instalador e da versão portátil lado a lado, o SHA-256 de cada pacote com um botão para copiar e o comando para verificar, e um layout para celular refeito em que todos os links continuam visíveis.
- Novos pacotes: AppImage do Linux para qualquer distribuição com glibc 2.35+ (com Qt, libarchive e OpenSSL incluídos; testado no Debian, Fedora, Arch e openSUSE na CI) com atualizações pelo app, `.zip` portátil do Windows, `.tar.gz` portátil do Linux e builds do macOS para Intel além do Apple Silicon, cada uma com um `.zip` portátil.
- Correção das instâncias Forge e NeoForge: as bibliotecas do carregador são combinadas por artefato e classificador, então o Forge mantém seu jar universal («Failed to find system mod: forge»). Verificado no jogo com Forge 26.3, 1.20.1, 1.16.5, 1.12.2, 1.8.9 e 1.7.10, NeoForge 26.3 e 1.21.1, Fabric e Quilt 26.3.
- As instâncias Forge, NeoForge, Fabric e Quilt instaladas com um launcher anterior são preparadas de novo na próxima vez que você tocar em Jogar (os arquivos baixados são reaproveitados).
- NeoForge 26.x e NeoForge para 1.20.1 estão disponíveis; Fabric e Quilt escolhem o carregador estável mais novo; Forge 1.7.10–1.12.2 instala pelo seu instalador antigo.
- As Versões perdidas iniciam com Java diretamente no Windows, no macOS e no Linux (sem Wine): a1.1.1, Alpha 1.0.16, 1.1 Patch 1, Ghost, 554e, NSSS e todas as outras versões arquivadas chegam aos seus menus. Os arquivos são verificados com SHA-256 e cada versão guarda seus mundos dentro da própria pasta.
- Importação de modpacks de `.mrpack`, `.zip` do CurseForge, Prism Launcher / MultiMC / PolyMC, app do CurseForge, FTB App, ATLauncher, GDLauncher, Modrinth App e Technic, com Forge, NeoForge, Fabric, Quilt ou vanilla. ZIPs dentro de pastas ou feitos no macOS e no Windows, nomes de arquivo em qualquer idioma, URLs espelho e arquivos opcionais do Modrinth, e pacotes de recursos e shaders do CurseForge são tratados.
- Quando há uma versão nova, aparece um botão «Atualizar para …» na barra lateral; ele baixa, verifica e instala a atualização sem visitar o site.
- Adicionado um gerenciador externo experimental do Bedrock com instalação em segundo plano. O Windows usa o BedrockLauncher; Linux e macOS usam o motor Android e exigem a compra no Google Play. A compra no Windows não vale para o Android. O jogo real continua não verificado em todas as plataformas.
- Adicionada a desinstalação das Versões perdidas com confirmação e envio para a lixeira.
- O progresso das operações foi para a barra de status inferior.
- Incluído o runtime do Visual C++ do Windows e adicionado um atalho na área de trabalho ao instalador por usuário.
- Adicionado Atualizar e reiniciar dentro do launcher. Os pacotes de cada versão são verificados com SHA-256, preparados e testados na inicialização antes de substituir os arquivos do programa instalado. Os arquivos anteriores do launcher e os downloads temporários são removidos depois de uma inicialização bem-sucedida. O NixOS compila a versão com o Nix; os caminhos compartilhados da store continuam sujeitos à coleta de lixo do Nix.
- O atualizador nunca remove instâncias, Versões perdidas, mundos, mods nem conteúdo baixado pelo usuário. As atualizações ficam bloqueadas enquanto há jogos ou instalações em andamento. Se a substituição falhar, os arquivos anteriores do programa são restaurados.
- O instalador do Windows detecta a pasta da instalação existente, incluindo o formato de registro do instalador anterior, e a atualiza no mesmo lugar.
- Adicionado um tour interativo de 14 passos na primeira execução, com controles destacados, Voltar/Próximo/Pular, e que pode ser repetido em Guia e tutorial.
- Quem usa a 1.0.0 precisa instalar a 1.1.0 uma vez para obter o atualizador integrado; as versões compatíveis seguintes se atualizam pelo launcher.
- Mantido o local existente dos dados do aplicativo.

O launcher não inclui o Minecraft, contas nem licenças do jogo. Os pacotes de Windows e macOS não são assinados. O binário do Linux é voltado ao Ubuntu 24.04 x64; outras distribuições podem compilar a partir do código, inclusive o NixOS pelo flake.
