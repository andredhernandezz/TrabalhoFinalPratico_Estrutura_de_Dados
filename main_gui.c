/* ============================================================================
 * main_gui.c - Interface grafica (Raylib) do Dado Burger.
 *
 * Camada de APRESENTACAO: nao reimplementa regra nenhuma. Toda acao do jogador
 * passa por jogoPrepara / jogoAdiciona / jogoRemove / jogoAbandona (jogo.c) e
 * todo estado exibido e LIDO das estruturas existentes (fila, pilha, lista
 * encadeada, estoque/heaps, hash, AVL). Nenhum TAD foi alterado.
 *
 * Controles:  letra = adicionar | Shift+letra = preparar | Backspace = remover
 *             Esc = abandonar   | mouse: clique no ingrediente / no "PREPARAR"
 * ========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "raylib.h"
#include "jogo.h"
#include "carregamento.h"
#include "historico.h"
#include "ordenacao.h"
#include "timer.h"

#define W 1280
#define H 720
#define LOG "medicoes.log"

/* ------------------------------ ingredientes ------------------------------ */
typedef struct {
    char s; const char *nome; Color cor; int prep, perec; IngredienteEstoque ing;
    Texture2D tex;                      /* assets/ico_<s>.png (opcional) */
} Ing;
static Ing ING[12] = {
    {'z',"Pão",      {232,162,60,255}, 0,0,0},       {'k',"Ketchup", {229,65,45,255},0,0,0},
    {'m',"Mostarda", {246,198,43,255}, 0,0,0},       {'p',"Picles",  {92,160,70,255},0,0,0},
    {'a',"Alface",   {88,184,74,255},  0,0,0},       {'s',"Q. Suíço",{251,226,122,255},1,0,QUEIJO_SUICO},
    {'q',"Cheddar",  {240,138,36,255}, 1,0,QUEIJO_CHEDDAR},{'c',"Cebola",{214,150,222,255},1,0,CEBOLA},
    {'t',"Tomate",   {240,85,61,255},  1,0,TOMATE},  {'n',"Carne",   {122,62,34,255},1,1,CARNE},
    {'o',"Ovo frito",{255,241,201,255},1,1,OVO},     {'b',"Bacon",   {201,69,58,255},1,1,BACON},
};
static Ing *acha(char s) { for (int i = 0; i < 12; i++) if (ING[i].s == s) return &ING[i]; return &ING[0]; }

/* --------------------------------- estado --------------------------------- */
typedef enum { T_MENU, T_COMO, T_HIST, T_NOME, T_JOGO, T_FIM } Tela;
static int quit = 0;
static Tela tela = T_MENU, proxT; static float fadeA = 0; static int fading = 0;

static NodoAvl *raiz; static ListaContig hist, hv; static int hvOk = 0, hOrd = 0, hScroll = 0;
static double hTempo = 0; static MetricasCarga carga; static const char *arqHist;
static Jogo jogo; static char nome[21]; static int nomeLen = 0;
static struct { StatusJogo st; int atend, total, pont, turno, n, m; long hits, misses, col, buscas, comps; double tBusca; } fim;

static char toast[96]; static float toastT = 0; static Color toastC;
static float shake = 0, flash = 0; static double tPush = -9;
static struct { Vector2 p, v; float life; Color c; } part[120];

static void aviso(const char *t, Color c) { snprintf(toast, sizeof toast, "%s", t); toastT = 2.2f; toastC = c; }
static void irPara(Tela t) { proxT = t; fading = 1; }
static void confete(float x, float y) {
    for (int i = 0, k = 0; i < 120 && k < 60; i++) if (part[i].life <= 0) {
        part[i].p = (Vector2){x, y};
        part[i].v = (Vector2){GetRandomValue(-300, 300), GetRandomValue(-520, -120)};
        part[i].life = 1.2f; part[i].c = ING[GetRandomValue(0, 11)].cor; k++;
    }
}

/* ------------------------------ helpers de UI ------------------------------ */
static const Color INK = {58,30,22,255}, CREAM = {255,244,214,255}, PANEL = {255,236,190,255};
static void Rr(float x, float y, float w, float h, float r, Color c) { DrawRectangleRounded((Rectangle){x,y,w,h}, r, 8, c); }
static void RrO(float x, float y, float w, float h, float r, Color c) { Rr(x-3,y-3,w+6,h+6,r,INK); Rr(x,y,w,h,r,c); }
static void TxtC(const char *t, float cx, float y, int sz, Color c) { DrawText(t, (int)(cx - MeasureText(t, sz) / 2), (int)y, sz, c); }
static int Btn(float x, float y, float w, float h, const char *t, Color c) {
    Rectangle r = {x, y, w, h}; int hot = CheckCollisionPointRec(GetMousePosition(), r);
    RrO(x, y + (hot ? -2 : 0), w, h, 0.4f, hot ? (Color){255,205,90,255} : c);
    TxtC(t, x + w / 2, y + h / 2 - 11 + (hot ? -2 : 0), 24, INK);
    return hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && fadeA < 0.2f;
}

/* camada lateral do hamburguer (vista de perfil) */
static void Camada(char s, float cx, float yBase, float w, int bun) {
    Ing *g = acha(s); float h = 22, y = yBase - h;
    if (s == 'z') {                                   /* pao de cima: cupula + gergelim */
        RrO(cx - w/2, y - 16, w, h + 16, 0.9f, g->cor);
        for (int i = -2; i <= 2; i++) DrawEllipse((int)(cx + i * w / 6), (int)(y - 2 - 6 * (i & 1)), 4, 2, (Color){255,241,201,255});
    } else if (bun) RrO(cx - w/2, y + 2, w, h - 2, 0.6f, (Color){242,184,92,255});
    else if (s == 'k' || s == 'm') RrO(cx - w/2 + 6, y + 8, w - 12, 9, 1.0f, g->cor);
    else RrO(cx - w/2 + (s=='a'||s=='p'?-4:0), y + 3, w + (s=='a'||s=='p'?8:0), h - 5, 0.5f, g->cor);
}

/* le a pilha (topo->base) e a receita (base->topo) para vetores locais */
static int lePilha(char *o) { int n = 0; for (NodoPilha *p = jogo.prato.topo; p && n < 40; p = p->prox) o[n++] = p->ingrediente;
    for (int i = 0; i < n / 2; i++) { char t = o[i]; o[i] = o[n-1-i]; o[n-1-i] = t; } return n; }
static int leReceita(char *o) { int n = 0; for (NodoLista *p = jogo.receita.ini; p && n < 40; p = p->prox) o[n++] = p->ingrediente; return n; }

static void Pilha(const char *v, int n, float cx, float yBase, const char *rec, int nr, double tp) {
    DrawEllipse((int)cx, (int)yBase + 8, 110, 16, (Color){255,255,255,255}); DrawEllipse((int)cx, (int)yBase + 12, 110, 14, (Color){210,200,190,255});
    DrawEllipse((int)cx, (int)yBase + 6, 100, 12, (Color){245,245,245,255});
    float y = yBase; Camada(' ', cx, y, 150, 1); y -= 22;
    for (int i = 0; i < n; i++) {
        float off = (i == n - 1 && tp >= 0 && tp < 0.2) ? -(0.2 - tp) * 1500 : 0;   /* animacao de queda */
        Camada(v[i], cx, y + off, 150, 0);
        if (rec && (i >= nr || rec[i] != v[i])) DrawText("x", (int)cx + 85, (int)(y - 20 + off), 22, RED);
        y -= (v[i] == 'k' || v[i] == 'm') ? 14 : 22;
    }
}

static void Rosto(float x, float y, float r, unsigned seed, float humor) {
    Color pele[4] = {{255,214,170,255},{230,170,120,255},{190,125,85,255},{120,80,55,255}};
    Color cab[4] = {{70,40,25,255},{30,25,25,255},{180,90,30,255},{20,20,20,255}};
    DrawCircle((int)x, (int)(y - r * .55f), r * .95f, cab[(seed >> 3) & 3]);
    DrawCircle((int)x, (int)y, r + 3, INK); DrawCircle((int)x, (int)y, r, pele[seed & 3]);
    DrawCircle((int)(x - r*.35f), (int)(y - r*.1f), r*.12f, INK); DrawCircle((int)(x + r*.35f), (int)(y - r*.1f), r*.12f, INK);
    if (humor > .5f) DrawCircleLines((int)x, (int)(y + r*.25f), r*.35f, INK);
    else if (humor > .25f) DrawLineEx((Vector2){x - r*.3f, y + r*.45f}, (Vector2){x + r*.3f, y + r*.45f}, 3, INK);
    else DrawCircleLines((int)x, (int)(y + r*.75f), r*.3f, INK);
}
static unsigned hnome(const char *s) { unsigned h = 7; while (*s) h = h * 31 + (unsigned char)*s++; return h; }

/* -------------------------------- partida -------------------------------- */
static void novaPartida(void) {
    if (!iniciaJogo(&jogo, raiz, MAX_CLIENTES, MAX_PACIENCIA)) { aviso("Erro ao iniciar a partida", RED); return; }
    toastT = 0; tPush = -9; irPara(T_JOGO);
}
static void encerra(void) {
    fim.st = jogo.status; fim.atend = jogo.atendidos; fim.total = jogo.totalClientes; fim.pont = jogo.pontuacao; fim.turno = jogo.turno;
    fim.hits = jogo.cache.hits; fim.misses = jogo.cache.misses; fim.col = jogo.cache.colisoes; fim.n = jogo.cache.n; fim.m = jogo.cache.m;
    fim.buscas = jogo.buscas.buscas; fim.comps = jogo.buscas.comparacoes; fim.tBusca = jogo.buscas.tempo;
    salvarPartida(arqHist, &hist, nome, jogo.pontuacao);                      /* historico em arquivo */
    MetricasHash mh = { jogo.cache.m, jogo.cache.n, jogo.cache.hits, jogo.cache.misses, jogo.cache.colisoes };
    metricasRelatorioPartida(LOG, &carga, &jogo.buscas, &mh);                 /* relatorio + log */
    destroiJogo(&jogo); irPara(T_FIM);
}
/* modo: 0 adicionar, 1 preparar, 2 remover */
static void acao(int modo, char s) {
    int at0 = jogo.atendidos, tam0 = jogo.prato.tamanho; ResultadoAcao r;
    if (modo == 2) r = jogoRemove(&jogo);
    else if (modo == 1) { if (!acha(s)->prep) { aviso("Esse ingrediente não precisa de preparo", ORANGE); shake = .3f; return; } r = jogoPrepara(&jogo, s); }
    else r = jogoAdiciona(&jogo, s);
    static const char *msg[] = {"", "Ingrediente inválido", "Sem estoque! Prepare antes", "Estoque cheio",
        "Hambúrguer já fechado! Remova o pão", "Nada para remover", "Partida encerrada", "Erro de memória", "Erro interno"};
    if (r != ACAO_SUCESSO) { aviso(msg[r], RED); shake = .3f; }
    else if (jogo.atendidos > at0) { aviso("Pedido entregue! +1", (Color){40,150,60,255}); flash = .5f; confete(560, 400); }
    else if (modo == 0 && jogo.prato.tamanho > tam0) {
        tPush = GetTime();
        if (s == 'z') { aviso("Hambúrguer errado! Remova o pão e corrija", RED); shake = .3f; }
    } else if (modo == 1) aviso(TextFormat("%s preparado", acha(s)->nome), (Color){40,150,60,255});
    if (jogo.status != JOGO_ANDAMENTO) encerra();
}

/* ---------------------------------- telas ---------------------------------- */
static void Chao(float off) {
    for (int y = -1; y < 10; y++) for (int x = -1; x < 17; x++)
        DrawRectangle(x * 80 + (int)off % 80, y * 80 + (int)off % 80, 80, 80, ((x + y) & 1) ? (Color){240,214,160,255} : (Color){228,196,138,255});
}
static void telaMenu(void) {
    double t = GetTime(); Chao((float)t * 12);
    for (int i = 0; i < 12; i++) { Camada(ING[i].s, 90 + i * 100, 130 + 18 * sinf((float)t * 2 + i), 70, 0); }
    TxtC("DADO", 640, 150, 110, INK); TxtC("BURGER", 640, 245, 80, (Color){214,60,40,255});
    DrawText("Estruturas de Dados · INF01203", 470, 345, 20, INK);
    if (Btn(490, 390, 300, 56, "Novo jogo", (Color){255,225,140,255})) irPara(T_NOME);
    if (Btn(490, 460, 300, 56, "Como jogar", (Color){255,225,140,255})) irPara(T_COMO);
    if (Btn(490, 530, 300, 56, "Histórico", (Color){255,225,140,255})) { hOrd = 0; hScroll = 0; irPara(T_HIST); }
    if (Btn(490, 600, 300, 56, "Sair", (Color){240,170,150,255})) quit = 1;
}
static void telaComo(void) {
    Chao(0); RrO(40, 30, 1200, 660, .05f, PANEL); TxtC("COMO JOGAR", 640, 45, 44, INK);
    const char *L[5] = {
        "Atenda todos os clientes da fila. Cada ação consome 1 turno e a paciência do cliente da frente diminui.",
        "Vitória: todos atendidos.   Derrota: a paciência do cliente da frente chega a zero.",
        "Monte o hambúrguer de baixo para cima, igual ao pedido, e feche com o pão de cima (z).",
        "O pão de baixo já está no prato. Errou? Remova o pão de cima (Backspace) e corrija.",
        "Preparar: Shift+letra ou COZINHAR | Adicionar: letra ou clique | Consultas não gastam turno." };
    for (int i = 0; i < 5; i++) DrawText(L[i], 70, 108 + i * 28, 20, INK);
    for (int i = 0; i < 12; i++) {
        float x = 70 + (i % 3) * 390, y = 250 + (i / 3) * 100; Ing *g = &ING[i];
        Camada(g->s, x + 40, y + 60, 70, 0); DrawText(TextFormat("[%c] %s", g->s, g->nome), (int)x + 100, (int)y + 10, 24, INK);
        DrawText(!g->prep ? "sem preparo, infinito" : g->perec ? "preparo + perecível (vence)" : "preparo, não perece", (int)x + 100, (int)y + 42, 18, DARKBROWN);
    }
    if (Btn(520, 625, 240, 50, "Voltar", (Color){255,225,140,255}) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}
static void ordena(int porPont) {
    if (hvOk) { destroiListaContig(&hv); hvOk = 0; }
    if (copiaListaContig(&hist, &hv) != 1) { hv.tamanho = 0; return; }       /* copia: fora do cronometro */
    Timer tm; timer_iniciar(&tm);
    int ok = porPont ? quicksortPontuacao(&hv) : quicksortAlfabetico(&hv);
    hTempo = timer_parar(&tm); hvOk = (ok == 1); hOrd = porPont ? 2 : 1; hScroll = 0;
    metricasLogOrdenacao(LOG, porPont ? "Quicksort (pontuacao)" : "Quicksort (alfabetico)", hv.tamanho, hTempo);
}
static void telaHist(void) {
    Chao(0); RrO(40, 30, 1200, 660, .05f, PANEL); TxtC("HISTÓRICO", 640, 40, 44, INK);
    if (Btn(80, 100, 260, 46, "Ordem alfabética", (Color){255,225,140,255})) ordena(0);
    if (Btn(360, 100, 300, 46, "Pontuação decrescente", (Color){255,225,140,255})) ordena(1);
    if (hOrd) DrawText(TextFormat("%d partidas · ordenação: %.6f s", hv.tamanho, hTempo), 700, 112, 20, INK);
    else DrawText("Escolha como ordenar.", 700, 112, 20, INK);
    if (hvOk) {
        hScroll -= (int)GetMouseWheelMove() * 3; if (hScroll > hv.tamanho - 12) hScroll = hv.tamanho - 12; if (hScroll < 0) hScroll = 0;
        for (int i = 0; i < 12 && hScroll + i < hv.tamanho; i++) {
            NovoHistorico h; obtemContig(&hv, hScroll + i, &h);
            DrawText(TextFormat("%d.", hScroll + i + 1), 100, 170 + i * 36, 24, INK);
            DrawText(h.nome, 190, 170 + i * 36, 24, INK); DrawText(TextFormat("%d", h.pontuacao), 700, 170 + i * 36, 24, (Color){214,60,40,255});
        }
        DrawText("Role o mouse para ver mais", 900, 650, 18, DARKBROWN);
    }
    if (Btn(80, 625, 200, 50, "Voltar", (Color){255,225,140,255}) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}
static void telaNome(void) {
    Chao(0); RrO(340, 220, 600, 280, .1f, PANEL); TxtC("Qual é o seu nome, atendente?", 640, 250, 32, INK);
    for (int c = GetCharPressed(); c; c = GetCharPressed()) if (c >= 32 && c < 127 && nomeLen < 20) { nome[nomeLen++] = (char)c; nome[nomeLen] = 0; }
    if (IsKeyPressed(KEY_BACKSPACE) && nomeLen) nome[--nomeLen] = 0;
    RrO(400, 320, 480, 56, .3f, WHITE); DrawText(nome, 416, 335, 30, INK);
    if (((int)(GetTime() * 2)) & 1) DrawRectangle(418 + MeasureText(nome, 30), 332, 3, 36, INK);
    if ((Btn(520, 410, 240, 54, "Começar!", (Color){150,220,120,255}) || IsKeyPressed(KEY_ENTER)) && nomeLen) novaPartida();
    if (IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}

static void telaJogo(void) {
    Chao(0); Cliente cl; frenteFila(&jogo.clientes, &cl);
    float sx = shake > 0 ? GetRandomValue(-4, 4) : 0; shake -= GetFrameTime();
    /* entrada de teclado: letra = adicionar, Shift+letra = preparar */
    for (int i = 0; i < 12; i++) if (IsKeyPressed(KEY_A + (ING[i].s - 'a')) && jogo.status == JOGO_ANDAMENTO)
        acao((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? 1 : 0, ING[i].s);
    if (jogo.status == JOGO_ANDAMENTO && IsKeyPressed(KEY_BACKSPACE)) acao(2, 0);
    if (jogo.status == JOGO_ANDAMENTO && IsKeyPressed(KEY_ESCAPE)) { jogoAbandona(&jogo); encerra(); }
    if (jogo.status != JOGO_ANDAMENTO) return;

    /* HUD */
    DrawRectangle(0, 0, W, 64, INK); DrawText("DADO BURGER", 20, 16, 34, (Color){255,205,90,255});
    DrawText(TextFormat("Turno %d", jogo.turno), 380, 20, 28, WHITE);
    DrawText(TextFormat("Atendidos %d/%d", jogo.atendidos, jogo.totalClientes), 560, 20, 28, WHITE);
    DrawText(TextFormat("Pontos %d", jogo.pontuacao), 840, 20, 28, WHITE);
    DrawText("Esc: abandonar", 1090, 24, 20, (Color){255,205,90,255});

    /* painel esquerdo: fila e paciência */
    RrO(16 + sx, 80, 330, 560, .05f, PANEL); float pf = (float)jogo.paciencia / jogo.pacienciaMax;
    Rosto(110, 170 + 4 * sinf((float)GetTime() * 4), 48, hnome(cl.nome), pf);
    DrawText(cl.nome, 175, 140, 24, INK); DrawText(TextFormat("Pedido #%d", cl.pedido.cod), 175, 170, 20, DARKBROWN);
    Rr(30, 240, 300, 22, .5f, INK); Rr(33, 243, 294 * pf, 16, .5f, pf > .5f ? GREEN : pf > .25f ? ORANGE : RED);
    TxtC(TextFormat("Paciência %d/%d", jogo.paciencia, jogo.pacienciaMax), 180, 268, 20, INK);
    DrawText("Receita:", 30, 310, 22, INK);
    char rec[40], pil[40]; int nr = leReceita(rec), np = lePilha(pil);
    int ok = 1, falha = np; for (int i = 0; i < np; i++) if (i >= nr || rec[i] != pil[i]) { ok = 0; falha = i; break; }
    for (int i = 0; i < nr; i++) { Ing *g = acha(rec[i]); float x = 40 + (i % 6) * 48, y = 345 + (i / 6) * 48;
        DrawCircle((int)x + 18, (int)y + 18, 21, INK); DrawCircle((int)x + 18, (int)y + 18, 18, g->cor);
        DrawText(TextFormat("%c", g->s), (int)x + 12, (int)y + 8, 22, i < np && ok ? WHITE : INK);
        if (i < np && ok) DrawText("v", (int)x + 28, (int)y - 8, 18, DARKGREEN); }
    DrawText(TextFormat("Fila: %d cliente(s)", tamanhoFila(&jogo.clientes)), 30, 470, 22, INK);
    int k = 0; for (Nodo *n = jogo.clientes.frente; n && k < 10; n = n->prox, k++)
        Rosto(55 + (k % 5) * 58, 530 + (k / 5) * 60, 22, hnome(n->cliente.nome), k == 0 ? pf : 1);

    /* painel central: pedido x montagem */
    RrO(360 + sx, 80, 480, 560, .05f, PANEL); TxtC("PEDIDO", 480, 95, 24, INK); TxtC("SEU HAMBÚRGUER", 720, 95, 24, INK);
    Pilha(rec, nr, 480, 560, NULL, 0, -1); Pilha(pil, np, 720, 560, rec, nr, GetTime() - tPush);
    if (np && pil[np-1] == 'z' && !ok) TxtC("Errado! Backspace para remover", 720, 130, 18, RED);
    if (flash > 0) { DrawRectangle(0, 0, W, H, Fade((Color){255,255,255,255}, flash)); flash -= GetFrameTime(); }

    /* painel direito: prateleira de ingredientes */
    RrO(856, 80, 408, 560, .05f, PANEL); char prox = (ok && np < nr) ? rec[np] : 0;
    Vector2 m = GetMousePosition();
    for (int i = 0; i < 12; i++) {
        Ing *g = &ING[i]; Rectangle r = {864 + (i % 3) * 134.f, 92 + (i / 3) * 136.f, 124, 126};
        int qtd = g->prep ? consultaEstoque(&jogo.estoque, g->ing) : -1;
        int hot = CheckCollisionPointRec(m, r);
        if (g->s == prox) Rr(r.x - 6, r.y - 6, r.width + 12, r.height + 12, .2f, Fade(GOLD, .5f + .5f * sinf((float)GetTime() * 6)));
        RrO(r.x, r.y, r.width, r.height, .15f, hot ? WHITE : CREAM);
        if (g->tex.id) DrawTexturePro(g->tex, (Rectangle){0,0,(float)g->tex.width,(float)g->tex.height}, (Rectangle){r.x + 30, r.y + 8, 64, 64}, (Vector2){0,0}, 0, WHITE);
        else { DrawCircle((int)r.x + 62, (int)r.y + 40, 30, INK); DrawCircle((int)r.x + 62, (int)r.y + 40, 27, g->cor); }
        DrawText(TextFormat("[%c]", g->s), (int)r.x + 6, (int)r.y + 6, 18, INK);
        TxtC(g->nome, r.x + 62, r.y + 76, 18, INK);
        if (!g->prep) TxtC("infinito", r.x + 62, r.y + 96, 16, DARKGREEN);
        else {
            Color qc = qtd > 0 ? DARKGREEN : RED; int v = g->perec ? consultaValidade(&jogo.estoque, g->ing, jogo.turno) : -1;
            DrawText(TextFormat("x%d", qtd), (int)r.x + 6, (int)r.y + 96, 18, qc);
            if (v >= 0) DrawText(TextFormat("vence %d", v), (int)r.x + 52, (int)r.y + 98, 15, v <= 1 ? RED : DARKBROWN);
        }
        /* cliques: parte de baixo (prep) = PREPARAR; resto = ADICIONAR */
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && fadeA < .2f) {
            if (g->prep && m.y > r.y + r.height - 26 && m.x > r.x + 50) acao(1, g->s); else acao(0, g->s);
        }
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) acao(1, g->s);
        if (jogo.status != JOGO_ANDAMENTO) return;
        if (g->prep) { Rr(r.x + 62, r.y + r.height - 26, 58, 22, .5f, hot ? ORANGE : (Color){240,170,90,255}); DrawText("COZINHAR", (int)r.x + 64, (int)r.y + r.height - 22, 11, INK); }
    }

    /* barra inferior */
    if (Btn(360, 652, 220, 50, "Remover (Bksp)", (Color){255,180,160,255})) acao(2, 0);
    if (jogo.status != JOGO_ANDAMENTO) return;
    DrawText("Letra: adicionar   Shift+letra / clique em COZINHAR: preparar", 600, 656, 18, INK);
    if (toastT > 0) { toastT -= GetFrameTime(); Rr(16, 652, 330, 50, .4f, toastC); TxtC(toast, 181, 668, 17, WHITE); }
}

static void telaFim(void) {
    Chao(0); RrO(280, 60, 720, 600, .05f, PANEL);
    int vit = fim.st == JOGO_VITORIA;
    TxtC(vit ? "VITÓRIA!" : fim.st == JOGO_DERROTA ? "O cliente desistiu..." : "Partida abandonada", 640, 80, 52, vit ? DARKGREEN : RED);
    TxtC(nome, 640, 150, 30, INK);
    TxtC(TextFormat("Pontuação: %d   (atendidos %d/%d em %d turnos)", fim.pont, fim.atend, fim.total, fim.turno - 1), 640, 200, 26, INK);
    DrawText("RELATÓRIO DE DESEMPENHO (completo em medicoes.log)", 320, 260, 22, INK);
    DrawText(TextFormat("AVL: %ld buscas, %ld comparações (média %.2f), %.6f s", fim.buscas, fim.comps, fim.buscas ? (double)fim.comps / fim.buscas : 0.0, fim.tBusca), 320, 300, 20, INK);
    long tot = fim.hits + fim.misses;
    DrawText(TextFormat("Hash: %ld hits, %ld misses, taxa %.1f%%", fim.hits, fim.misses, tot ? 100.0 * fim.hits / tot : 0.0), 320, 335, 20, INK);
    DrawText(TextFormat("Hash: fator de carga %.2f (n=%d, m=%d), colisões %ld", fim.m ? (double)fim.n / fim.m : 0.0, fim.n, fim.m, fim.col), 320, 370, 20, INK);
    DrawText(TextFormat("Carga: %d nós, altura %d, %d rotações, %.6f s", carga.nos, carga.altura, carga.rotacoes, carga.tempoCarga), 320, 405, 20, INK);
    if (Btn(360, 520, 270, 58, "Jogar de novo", (Color){150,220,120,255}) || IsKeyPressed(KEY_ENTER)) novaPartida();
    if (Btn(650, 520, 270, 58, "Menu", (Color){255,225,140,255}) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}

/* ---------------------------------- main ---------------------------------- */
int main(int argc, char *argv[]) {
    if (argc < 3) { fprintf(stderr, "Uso: %s <pedidos> <historico>\n", argv[0]); return 1; }
    srand((unsigned)time(NULL)); arqHist = argv[2]; raiz = NULL; inicializaListaContig(&hist); avlZerarMetricas();
    Timer t; timer_iniciar(&t); int lidos = carregarPedidos(argv[1], &raiz); double tc = timer_parar(&t);   /* mesma carga do modo terminal */
    if (lidos < 0) { fprintf(stderr, "Erro ao carregar pedidos.\n"); return 1; }
    int nh = carregarHistorico(argv[2], &hist); Contador mc = avlObterMetricas();
    carga = (MetricasCarga){ mc.nosInserido, avlAltura(raiz), mc.rotacoes, tc, nh }; metricasLogCarga(LOG, &carga);

    InitWindow(W, H, "Dado Burger"); SetTargetFPS(60); SetExitKey(KEY_NULL);
    for (int i = 0; i < 12; i++) { char p[64]; snprintf(p, sizeof p, "assets/ico_%c.png", ING[i].s);
        if (FileExists(p)) ING[i].tex = LoadTexture(p); }          /* icones opcionais */
    while (!quit && !WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing(); ClearBackground(CREAM);
        switch (tela) { case T_MENU: telaMenu(); break; case T_COMO: telaComo(); break; case T_HIST: telaHist(); break;
                        case T_NOME: telaNome(); break; case T_JOGO: telaJogo(); break; case T_FIM: telaFim(); break; }
        for (int i = 0; i < 120; i++) if (part[i].life > 0) {            /* confete */
            part[i].life -= dt; part[i].v.y += 900 * dt; part[i].p.x += part[i].v.x * dt; part[i].p.y += part[i].v.y * dt;
            DrawRectangle((int)part[i].p.x, (int)part[i].p.y, 8, 8, part[i].c); }
        if (fading) { fadeA += dt * 4; if (fadeA >= 1) { tela = proxT; fading = 0; fadeA = 1; if (tela == T_NOME) { nome[0] = 0; nomeLen = 0; } } }
        else if (fadeA > 0) fadeA -= dt * 4;
        if (fadeA > 0) DrawRectangle(0, 0, W, H, Fade(BLACK, fadeA));
        EndDrawing();
    }
    if (tela == T_JOGO && jogo.status == JOGO_ANDAMENTO) destroiJogo(&jogo);
    CloseWindow(); if (hvOk) destroiListaContig(&hv);
    avlDestruir(raiz); destroiListaContig(&hist); return 0;
}
