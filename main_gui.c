/* ============================================================================
 * main_gui.c - Interface grafica (Raylib) do Dado Burger.
 *
 * Camada de APRESENTACAO: nao reimplementa regra nenhuma. Toda acao do jogador
 * passa por jogoPrepara / jogoAdiciona / jogoRemove / jogoAbandona (jogo.c) e
 * todo estado exibido e LIDO das estruturas existentes (fila, pilha, lista
 * encadeada, estoque/heaps, hash, AVL). Nenhum TAD foi alterado.
 *
 * Visual: estilo "pixel cartoon" - fundo listrado bege, paineis cremes com
 * contorno marrom, cabecalho escuro. Fonte opcional: assets/font.ttf (qualquer
 * TTF pixelado); sem ela usa a fonte padrao da Raylib.
 *
 * Controles:  letra = adicionar | Shift+letra = preparar | Backspace = remover
 *             Esc = abandonar   | mouse: clique no ingrediente / no "COZINHAR"
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
    Texture2D tex, lay;                 /* assets/ico_<s>.png e lay_<s>.png (opcionais) */
} Ing;
static Ing ING[12] = {
    {'z',"Pão",      {232,162,60,255}, 0,0,0},       {'k',"Ketchup", {229,65,45,255},0,0,0},
    {'m',"Mostarda", {246,198,43,255}, 0,0,0},       {'p',"Picles",  {92,160,70,255},0,0,0},
    {'a',"Alface",   {88,184,74,255},  0,0,0},       {'s',"Q. Suíço",{251,226,122,255},1,0,QUEIJO_SUICO},
    {'q',"Cheddar",  {240,138,36,255}, 1,0,QUEIJO_CHEDDAR},{'c',"Cebola",{214,150,222,255},1,0,CEBOLA},
    {'t',"Tomate",   {240,85,61,255},  1,0,TOMATE},  {'n',"Carne",   {122,62,34,255},1,1,CARNE},
    {'o',"Ovo frito",{255,241,201,255},1,1,OVO},     {'b',"Bacon",   {201,69,58,255},1,1,BACON},
};
static Texture2D baseTex;               /* assets/lay_base.png: pao de baixo */
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

/* ------------------------------ paleta / tema ------------------------------ */
static const Color INK   = {58,30,22,255};      /* contorno / texto escuro */
static const Color CREAM = {255,244,214,255};
static const Color PANEL = {253,236,196,255};   /* painel grande */
static const Color CARDC = {255,246,224,255};   /* cartao claro */
static const Color OURO  = {255,205,50,255};
static const Color ACC   = {240,150,50,255};
static const Color MINT  = {40,190,90,255};
static const Color MUTE  = {140,100,70,255};
static const Color SUB   = {246,222,172,255};
static const Color SESAMO = {255,241,201,255};
static const Color BAD   = {220,60,55,255};

static Color Lt(Color c, float t) { return (Color){(unsigned char)(c.r + (255 - c.r) * t), (unsigned char)(c.g + (255 - c.g) * t), (unsigned char)(c.b + (255 - c.b) * t), 255}; }
static Color Dk(Color c, float t) { return (Color){(unsigned char)(c.r * (1 - t)), (unsigned char)(c.g * (1 - t)), (unsigned char)(c.b * (1 - t)), 255}; }

/* ------------------------------- primitivas ------------------------------- */
static void Rr(float x, float y, float w, float h, float r, Color c) { DrawRectangleRounded((Rectangle){x, y, w, h}, r, 8, c); }
static void RrB(float x, float y, float w, float h, float r, Color f, Color b, float t) { Rr(x - t, y - t, w + 2 * t, h + 2 * t, r, b); Rr(x, y, w, h, r, f); }
static void RrC(float x, float y, float w, float h, float r, Color f, Color b) { RrB(x, y, w, h, r, f, b, 3); }
static void RrO(float x, float y, float w, float h, float r, Color c) { RrC(x, y, w, h, r, c, INK); }
static void Painel(float x, float y, float w, float h, float r, Color f) {
    Rr(x - 3, y + 2, w + 6, h + 6, r, (Color){0,0,0,45});   /* sombra */
    RrC(x, y, w, h, r, f, INK);
}

/* ---------------------------------- texto ---------------------------------- */
static Font fonte; static int fonteOk = 0;
static float Sp(float sz) { return fonteOk ? sz * 0.05f : (sz < 10 ? 1.f : (float)((int)(sz / 10))); }
static void Txt(const char *t, float x, float y, float sz, Color c) { DrawTextEx(fonte, t, (Vector2){x, y}, sz, Sp(sz), c); }
static float TW(const char *t, float sz) { return MeasureTextEx(fonte, t, sz, Sp(sz)).x; }
static void TxtC(const char *t, float cx, float y, float sz, Color c) { Txt(t, cx - TW(t, sz) / 2, y, sz, c); }
static void TxtR(const char *t, float rx, float y, float sz, Color c) { Txt(t, rx - TW(t, sz), y, sz, c); }
static void TxtCS(const char *t, float cx, float y, float sz, Color c) {      /* titulo com contorno */
    float o = sz > 60 ? 4 : 3;
    for (int dx = -1; dx <= 1; dx++) for (int dy = -1; dy <= 1; dy++) if (dx || dy) TxtC(t, cx + dx * o, y + dy * o + 2, sz, INK);
    TxtC(t, cx, y, sz, c);
}

/* ------------------------------ efeitos / UI ------------------------------ */
static void aviso(const char *t, Color c) { snprintf(toast, sizeof toast, "%s", t); toastT = 2.2f; toastC = c; }
static void irPara(Tela t) { proxT = t; fading = 1; }
static void confete(float x, float y) {
    for (int i = 0, k = 0; i < 120 && k < 60; i++) if (part[i].life <= 0) {
        part[i].p = (Vector2){x, y};
        part[i].v = (Vector2){GetRandomValue(-300, 300), GetRandomValue(-520, -120)};
        part[i].life = 1.2f; part[i].c = ING[GetRandomValue(0, 11)].cor; k++;
    }
}
static struct { char t[24]; float x, y, life; } popups[6];
static void addPop(const char *t) { for (int i = 0; i < 6; i++) if (popups[i].life <= 0) { snprintf(popups[i].t, 24, "%s", t); popups[i].x = 600; popups[i].y = 330; popups[i].life = 1.4f; return; } }
static void desenhaPops(float dt) {
    for (int i = 0; i < 6; i++) if (popups[i].life > 0) { popups[i].life -= dt; popups[i].y -= 70 * dt; float a = popups[i].life > 1 ? 1 : popups[i].life;
        TxtC(popups[i].t, popups[i].x + 3, popups[i].y + 3, 40, Fade(INK, a));
        TxtC(popups[i].t, popups[i].x, popups[i].y, 40, Fade(MINT, a)); }
}
static void Fundo(void) {                       /* listras verticais bege */
    for (int x = 0, i = 0; x < W; x += 48, i++)
        DrawRectangle(x, 0, 48, H, (i & 1) ? (Color){231,204,150,255} : (Color){241,221,176,255});
}
/* st: 0 laranja, 1 creme, 2 rosa/vermelho, 3 verde */
static int Btn(float x, float y, float w, float h, const char *t, int st) {
    int hot = CheckCollisionPointRec(GetMousePosition(), (Rectangle){x, y, w, h}); float dy = hot ? -3 : 0;
    Color f = st == 0 ? (Color){255,168,64,255} : st == 2 ? (Color){255,190,180,255} : st == 3 ? (Color){130,215,130,255} : CREAM;
    Color b = st == 2 ? (Color){180,70,70,255} : INK; if (hot) f = Lt(f, .25f);
    Rr(x, y + 6, w, h, .25f, (Color){0,0,0,70}); RrC(x, y + dy, w, h, .25f, f, b);
    TxtC(t, x + w / 2, y + h / 2 - 12 + dy, 24, INK);
    return hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && fadeA < 0.2f;
}

/* ---------- arte "cartoon" desenhada por codigo (sem depender de PNG) ---------- */
static void Ov(float x, float y, float rx, float ry, Color c) { DrawEllipse((int)x, (int)y, rx + 2, ry + 2, INK); DrawEllipse((int)x, (int)y, rx, ry, c); }
static void Fita(float x0, float x1, float y, float amp, float esp, Color c, float ph) {   /* tira ondulada (bacon / mostarda) */
    for (int i = 0; i < 14; i++) { float a = x0 + (x1 - x0) * i / 14.f, b = x0 + (x1 - x0) * (i + 1) / 14.f;
        DrawLineEx((Vector2){a, y + amp * sinf(ph + i * .9f)}, (Vector2){b, y + amp * sinf(ph + (i + 1) * .9f)}, esp, c); }
}

/* icone frontal 64x64 (escala sz/64), centrado em (cx,cy) */
static void Icone(char s, float cx, float cy, float sz) {
    float k = sz / 64.f; Color c = acha(s)->cor; Texture2D *T = &acha(s)->tex;
    if (T->id) { DrawTexturePro(*T, (Rectangle){0,0,(float)T->width,(float)T->height}, (Rectangle){cx - sz/2, cy - sz/2, sz, sz}, (Vector2){0,0}, 0, WHITE); return; }
    switch (s) {
    case 'z': Ov(cx, cy - 2*k, 28*k, 22*k, c); RrO(cx - 26*k, cy + 8*k, 52*k, 12*k, .6f, Lt(c, .25f));
        { float p[6][2] = {{-12,-12},{0,-17},{12,-10},{-6,-3},{8,-1},{-19,-3}};
          for (int i = 0; i < 6; i++) DrawEllipse((int)(cx + p[i][0]*k), (int)(cy + p[i][1]*k), 4*k, 2*k, SESAMO); }
        DrawEllipse((int)(cx - 14*k), (int)(cy - 14*k), 6*k, 3*k, Lt(c, .5f)); break;
    case 'k': case 'm':
        RrO(cx - 15*k, cy - 8*k, 30*k, 36*k, .4f, c); RrO(cx - 7*k, cy - 20*k, 14*k, 14*k, .3f, Lt(c, .1f));
        RrO(cx - 4*k, cy - 30*k, 8*k, 10*k, .3f, Dk(c, .3f)); RrO(cx - 11*k, cy + 2*k, 22*k, 16*k, .3f, CREAM);
        DrawCircle((int)cx, (int)(cy + 10*k), 4*k, c); break;
    case 'p':
        for (int j = 0; j < 2; j++) { float px = cx + (j ? 10 : -10)*k, py = cy + (j ? 10 : -10)*k;
            DrawCircle((int)px, (int)py, 22*k + 2, INK); DrawCircle((int)px, (int)py, 22*k, (Color){63,143,58,255});
            DrawCircle((int)px, (int)py, 18*k, (Color){166,216,106,255}); DrawCircle((int)px, (int)py, 12*k, (Color){205,235,139,255});
            for (int a = 0; a < 6; a++) DrawCircle((int)(px + 7*k*cosf(a * 1.047f)), (int)(py + 7*k*sinf(a * 1.047f)), 2*k, (Color){244,241,181,255}); }
        break;
    case 'a':
        for (int i = 0; i < 7; i++) DrawCircle((int)(cx + 19*k*cosf(i * .8976f)), (int)(cy + 14*k*sinf(i * .8976f)), 13*k + 2, INK);
        for (int i = 0; i < 7; i++) DrawCircle((int)(cx + 19*k*cosf(i * .8976f)), (int)(cy + 14*k*sinf(i * .8976f)), 13*k, c);
        DrawCircle((int)cx, (int)cy, 19*k, Lt(c, .1f)); DrawLineEx((Vector2){cx, cy + 22*k}, (Vector2){cx, cy - 18*k}, 3*k, Lt(c, .6f));
        DrawLineEx((Vector2){cx, cy + 6*k}, (Vector2){cx - 16*k, cy - 8*k}, 2.5f*k, Lt(c, .6f)); DrawLineEx((Vector2){cx, cy + 6*k}, (Vector2){cx + 16*k, cy - 8*k}, 2.5f*k, Lt(c, .6f)); break;
    case 's': case 'q':
        RrO(cx - 25*k, cy - 21*k, 50*k, 44*k, .12f, c); Rr(cx + 9*k, cy + 6*k, 15*k, 15*k, .2f, Dk(c, .15f));
        if (s == 's') { float h[5][3] = {{-10,-8,6},{8,-12,4.5f},{4,8,7},{-14,10,3.5f},{14,-2,3}};
            for (int i = 0; i < 5; i++) { DrawCircle((int)(cx + h[i][0]*k), (int)(cy + h[i][1]*k), h[i][2]*k, Dk(c, .3f)); DrawCircle((int)(cx + h[i][0]*k + k), (int)(cy + h[i][1]*k + k), h[i][2]*k*.7f, Dk(c, .45f)); } }
        else DrawLineEx((Vector2){cx - 18*k, cy - 12*k}, (Vector2){cx + 8*k, cy - 12*k}, 3*k, Lt(c, .5f));
        break;
    case 'c':
        DrawCircle((int)cx, (int)cy, 25*k + 2, INK); DrawCircle((int)cx, (int)cy, 25*k, c);
        DrawCircle((int)cx, (int)cy, 20*k, Lt(c, .6f)); DrawCircle((int)cx, (int)cy, 16*k, c); DrawCircle((int)cx, (int)cy, 11*k, Lt(c, .6f));
        DrawCircle((int)cx, (int)cy, 7*k, c); DrawCircle((int)cx, (int)cy, 3*k, Lt(c, .6f)); break;
    case 't':
        DrawCircle((int)cx, (int)(cy + 3*k), 25*k + 2, INK); DrawCircle((int)cx, (int)(cy + 3*k), 25*k, c);
        DrawEllipse((int)(cx - 10*k), (int)(cy - 6*k), 7*k, 3.5f*k, Lt(c, .6f));
        for (int i = -2; i <= 2; i++) DrawEllipse((int)(cx + i*6*k), (int)(cy - 20*k + abs(i)*2*k), 6*k, 2.5f*k, (Color){60,150,60,255});
        DrawCircle((int)cx, (int)(cy - 22*k), 3*k, (Color){60,150,60,255}); break;
    case 'n':
        Ov(cx, cy, 28*k, 20*k, c);
        for (int i = -1; i <= 1; i++) DrawLineEx((Vector2){cx + i*12*k - 6*k, cy + 12*k}, (Vector2){cx + i*12*k + 6*k, cy - 12*k}, 3*k, Lt(c, .35f));
        DrawEllipse((int)(cx - 14*k), (int)(cy - 11*k), 6*k, 2.5f*k, Lt(c, .45f)); break;
    case 'o':
        { float b[3][3] = {{-12,4,16},{10,6,15},{0,-8,17}};
          for (int i = 0; i < 3; i++) DrawCircle((int)(cx + b[i][0]*k), (int)(cy + b[i][1]*k), b[i][2]*k + 2, INK);
          for (int i = 0; i < 3; i++) DrawCircle((int)(cx + b[i][0]*k), (int)(cy + b[i][1]*k), b[i][2]*k, (Color){255,252,240,255}); }
        DrawCircle((int)(cx - 2*k), (int)(cy + 2*k), 10*k + 2, Dk((Color){246,190,40,255}, .4f)); DrawCircle((int)(cx - 2*k), (int)(cy + 2*k), 10*k, (Color){246,190,40,255});
        DrawCircle((int)(cx - 5*k), (int)(cy - 1*k), 3*k, (Color){255,235,150,255}); break;
    case 'b':
        for (int p = 0; p < 2; p++) for (int j = 0; j < 2; j++) {
            float off = (j ? 10 : -10)*k;
            if (p == 0) Fita(cx - 27*k, cx + 27*k, cy + off, 4*k, 15*k + 4, INK, j * 1.5f);
            else { Fita(cx - 27*k, cx + 27*k, cy + off, 4*k, 15*k, c, j * 1.5f); Fita(cx - 27*k, cx + 27*k, cy + off, 4*k, 4*k, (Color){245,190,170,255}, j * 1.5f); } }
        break;
    }
}

/* camada lateral do hamburguer (vista de perfil), cada ingrediente com seu detalhe */
static void Camada(char s, float cx, float yBase, float w, int bun) {
    Ing *g = acha(s); Color c = g->cor; float h = 22, y = yBase - h, x0 = cx - w / 2;
    Texture2D *T = bun ? &baseTex : &g->lay;                 /* sprite lateral, se existir */
    if (T->id) { float dh = T->height * w / T->width; DrawTexturePro(*T, (Rectangle){0,0,(float)T->width,(float)T->height}, (Rectangle){x0, yBase - dh + 4, w, dh}, (Vector2){0,0}, 0, WHITE); return; }
    if (bun) { RrO(x0, y, w, h, .6f, (Color){242,184,92,255}); DrawLineEx((Vector2){x0 + 14, y + 6}, (Vector2){x0 + 50, y + 6}, 2, Lt((Color){242,184,92,255}, .5f)); return; }
    switch (s) {
    case 'z': RrO(x0, y - 18, w, h + 18, .9f, c);
        for (int i = -2; i <= 2; i++) DrawEllipse((int)(cx + i * w / 6), (int)(y - 4 - 6 * (i & 1)), 4, 2, SESAMO);
        DrawEllipse((int)(cx - w / 3), (int)(y - 10), 10, 3, Lt(c, .5f)); break;
    case 'k': case 'm': Fita(x0 + 8, x0 + w - 8, y + 11, 3, 11, INK, 0); Fita(x0 + 8, x0 + w - 8, y + 11, 3, 7, c, 0);
        for (int i = 0; i < 3; i++) { float dx = x0 + 28 + i * (w - 56) / 2; DrawCircle((int)dx, (int)y + 19, 5, INK); DrawCircle((int)dx, (int)y + 19, 3.5f, c); } break;
    case 'p': for (int i = 0; i < 5; i++) { float ex = x0 + 18 + i * (w - 36) / 4;
            Ov(ex, y + 11, 16, 8, (Color){80,160,70,255}); DrawEllipse((int)ex, (int)y + 11, 10, 4.5f, (Color){175,220,120,255}); } break;
    case 'a': for (int i = 0; i < 16; i++) { Vector2 p0 = {x0 + i * w / 16, y + 11 + ((i & 1) ? 6 : -6)}, p1 = {x0 + (i + 1) * w / 16, y + 11 + ((i & 1) ? -6 : 6)}; DrawLineEx(p0, p1, 13, INK); }
        for (int i = 0; i < 16; i++) { Vector2 p0 = {x0 + i * w / 16, y + 11 + ((i & 1) ? 6 : -6)}, p1 = {x0 + (i + 1) * w / 16, y + 11 + ((i & 1) ? -6 : 6)}; DrawLineEx(p0, p1, 9, c); } break;
    case 's': case 'q': RrO(x0 - 2, y + 5, w + 4, h - 10, .3f, c);
        RrO(x0 + w - 36, y + h - 8, 20, 10, .6f, c);
        if (s == 's') for (int i = 0; i < 3; i++) DrawCircle((int)(x0 + 30 + i * (w - 60) / 2), (int)(y + 11), 3, Dk(c, .35f));
        else DrawLineEx((Vector2){x0 + 8, y + 8}, (Vector2){x0 + 40, y + 8}, 2, Lt(c, .5f)); break;
    case 'c': for (int i = 0; i < 6; i++) RrO(x0 + 4 + i * (w - 36) / 5, y + 6 + (i & 1) * 3, 28, 12, 1.f, (Color){190,130,210,255}); break;
    case 't': for (int i = 0; i < 4; i++) RrO(x0 + 6 + i * (w - 56) / 3, y + 5 + (i & 1) * 3, 44, 14, 1.f, c); break;
    case 'n': RrO(x0, y - 1, w, h + 2, .5f, c);
        for (int i = 0; i < 6; i++) DrawLineEx((Vector2){x0 + 14 + i * (w - 28) / 5, y + 14}, (Vector2){x0 + 20 + i * (w - 28) / 5, y + 4}, 2.5f, Lt(c, .55f)); break;
    case 'o': RrO(x0 + 6, y + 8, w - 12, 8, 1.0f, (Color){255,252,240,255});
        Ov(cx, y + 9, 22, 9, (Color){246,190,40,255}); DrawEllipse((int)(cx - 8), (int)(y + 6), 6, 2.5f, (Color){255,235,150,255}); break;
    case 'b': Fita(x0 + 4, x0 + w - 4, y + 12, 4, 14, INK, 0); Fita(x0 + 4, x0 + w - 4, y + 12, 4, 10, c, 0); Fita(x0 + 4, x0 + w - 4, y + 12, 4, 3, (Color){245,190,170,255}, 0); break;
    default: RrO(x0, y + 3, w, h - 5, 0.5f, c);
    }
}

/* le a pilha (topo->base) e a receita (base->topo) para vetores locais */
static int lePilha(char *o) { int n = 0; for (NodoPilha *p = jogo.prato.topo; p && n < 40; p = p->prox) o[n++] = p->ingrediente;
    for (int i = 0; i < n / 2; i++) { char t = o[i]; o[i] = o[n-1-i]; o[n-1-i] = t; } return n; }
static int leReceita(char *o) { int n = 0; for (NodoLista *p = jogo.receita.ini; p && n < 40; p = p->prox) o[n++] = p->ingrediente; return n; }

/* prato + pao de baixo + camadas. rec != NULL marca com "x" as camadas erradas */
static void DesenhaPrato(const char *v, int n, float cx, float yBase, const char *rec, int nr, double tp) {
    DrawEllipse((int)cx, (int)yBase + 15, 114, 15, (Color){0,0,0,35});
    DrawEllipse((int)cx, (int)yBase + 9, 112, 16, (Color){196,190,204,255});
    DrawEllipse((int)cx, (int)yBase + 6, 112, 15, (Color){250,248,245,255});
    DrawEllipse((int)cx, (int)yBase + 5, 94, 9, (Color){236,234,240,255});
    float y = yBase; Camada(' ', cx, y, 150, 1); y -= 22;
    for (int i = 0; i < n; i++) {
        float off = (i == n - 1 && tp >= 0 && tp < 0.2) ? -(0.2 - tp) * 1500 : 0;   /* animacao de queda */
        Camada(v[i], cx, y + off, 150, 0);
        if (rec && (i >= nr || rec[i] != v[i])) Txt("x", cx + 84, y - 20 + off, 24, BAD);
        y -= (v[i] == 'k' || v[i] == 'm') ? 16 : 22;
    }
}

static void Rosto(float x, float y, float r, unsigned seed, float humor) {
    Color pele[4] = {{255,214,170,255},{230,170,120,255},{190,125,85,255},{120,80,55,255}};
    Color cab[4] = {{70,40,25,255},{30,25,25,255},{180,90,30,255},{20,20,20,255}};
    DrawCircle((int)x, (int)(y - r * .55f), r * .95f + 2, INK);
    DrawCircle((int)x, (int)(y - r * .55f), r * .95f, cab[(seed >> 3) & 3]);
    DrawCircle((int)x, (int)y, r + 3, INK); DrawCircle((int)x, (int)y, r, pele[seed & 3]);
    DrawCircle((int)(x - r*.35f), (int)(y - r*.1f), r*.12f, INK); DrawCircle((int)(x + r*.35f), (int)(y - r*.1f), r*.12f, INK);
    if (humor > .5f) DrawRing((Vector2){x, y + r*.25f}, r*.35f - 1.5f, r*.35f + 1.5f, 0, 360, 24, INK);
    else if (humor > .25f) DrawLineEx((Vector2){x - r*.3f, y + r*.45f}, (Vector2){x + r*.3f, y + r*.45f}, 3, INK);
    else DrawRing((Vector2){x, y + r*.75f}, r*.3f - 1.5f, r*.3f + 1.5f, 180, 360, 16, INK);
}
static unsigned hnome(const char *s) { unsigned h = 7; while (*s) h = h * 31 + (unsigned char)*s++; return h; }

/* -------------------------------- partida -------------------------------- */
static void novaPartida(void) {
    if (!iniciaJogo(&jogo, raiz, MAX_CLIENTES, MAX_PACIENCIA)) { aviso("Erro ao iniciar a partida", RED); return; }
    toastT = 0; tPush = -9; irPara(T_JOGO);
}

static void encerra(void) {
    fim.st = jogo.status;
    fim.atend = jogo.atendidos;
    fim.total = jogo.totalClientes;
    fim.pont = jogo.pontuacao;
    fim.turno = jogo.turno;
    fim.hits = jogo.cache.hits;
    fim.misses = jogo.cache.misses;
    fim.col = jogo.cache.colisoes;
    fim.n = jogo.cache.n;
    fim.m = jogo.cache.m;
    fim.buscas = jogo.buscas.buscas;
    fim.comps = jogo.buscas.comparacoes;
    fim.tBusca = jogo.buscas.tempo;

    int historicoSalvo = salvarPartida(
        arqHist, &hist, nome, jogo.pontuacao
    );

    MetricasHash mh = {
        jogo.cache.m,
        jogo.cache.n,
        jogo.cache.hits,
        jogo.cache.misses,
        jogo.cache.colisoes
    };

    int metricasSalvas = metricasRelatorioPartida(
        LOG, &carga, &jogo.buscas, &mh
    );

    destroiJogo(&jogo);
    irPara(T_FIM);

    if (!historicoSalvo && !metricasSalvas) {
        aviso("Erro ao salvar historico e metricas!", RED);
    }
    else if (!historicoSalvo) {
        aviso("Erro ao salvar historico!", RED);
    }
    else if (!metricasSalvas) {
        aviso("Erro ao salvar metricas!", RED);
    }
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
    else if (jogo.atendidos > at0) { aviso("Pedido entregue! +1", (Color){40,150,60,255}); flash = .3f; confete(560, 400); addPop("+1 cliente!"); }
    else if (modo == 0 && jogo.prato.tamanho > tam0) {
        tPush = GetTime();
        if (s == 'z') { aviso("Hambúrguer errado! Remova o pão e corrija", RED); shake = .3f; }
    } else if (modo == 1) aviso(TextFormat("%s preparado", acha(s)->nome), (Color){40,150,60,255});
    if (jogo.status != JOGO_ANDAMENTO) encerra();
}

/* ---------------------------------- telas ---------------------------------- */
static void telaMenu(void) {
    Fundo(); double t = GetTime();
    for (int i = 0; i < 12; i++) Icone(ING[i].s, 90 + i * 100, 100 + 20 * sinf((float)t * 1.6f + i) + (i & 1) * 14, 62);
    TxtCS("DADO", 640, 165, 112, OURO); TxtCS("BURGER", 640, 272, 80, ACC);
    RrC(500, 365, 280, 6, 1.f, ACC, INK); TxtC("Atenda todos os clientes antes que percam a paciência!", 640, 388, 20, INK);
    if (Btn(490, 440, 300, 56, "Novo jogo", 0)) irPara(T_NOME);
    if (Btn(490, 510, 300, 56, "Como jogar", 1)) irPara(T_COMO);
    if (Btn(490, 580, 300, 56, "Histórico", 1)) { hOrd = 0; hScroll = 0; irPara(T_HIST); }
    if (Btn(490, 650, 300, 46, "Sair", 2)) quit = 1;
    Txt("UFRGS · INF01203 Estruturas de Dados", 20, 694, 16, INK);
}
static void telaComo(void) {
    Fundo(); Painel(40, 24, 1200, 672, .03f, PANEL); TxtCS("COMO JOGAR", 640, 38, 40, OURO);
    const char *L[5] = {
        "Atenda todos os clientes da fila. Cada ação consome 1 turno e a paciência do cliente da frente diminui.",
        "Vitória: todos atendidos.   Derrota: a paciência do cliente da frente chega a zero.",
        "Monte o hambúrguer de baixo para cima, igual ao pedido, e feche com o pão de cima (z).",
        "O pão de baixo já está no prato. Errou? Remova o pão de cima (Backspace) e corrija.",
        "Preparar: Shift+letra ou COZINHAR | Adicionar: letra ou clique | Consultas não gastam turno." };
    for (int i = 0; i < 5; i++) { DrawCircle(78, 118 + i * 28, 5, ACC); Txt(L[i], 96, 108 + i * 28, 18, INK); }
    for (int i = 0; i < 12; i++) {
        float x = 56 + (i % 3) * 390, y = 262 + (i / 3) * 92; Ing *g = &ING[i];
        RrO(x, y, 372, 80, .2f, CARDC); DrawCircle((int)x + 44, (int)y + 40, 32, Fade(g->cor, .25f)); Icone(g->s, x + 44, y + 40, 52);
        Txt(TextFormat("%s  [%c]", g->nome, g->s), x + 94, y + 10, 22, INK);
        const char *tag = !g->prep ? "livre" : g->perec ? "cozinhar · perece" : "cozinhar";
        Color tc = !g->prep ? (Color){30,150,90,255} : g->perec ? (Color){200,60,60,255} : (Color){200,110,20,255};
        Rr(x + 94, y + 44, TW(tag, 15) + 20, 24, 1.f, Fade(tc, .22f)); Txt(tag, x + 104, y + 48, 15, tc);
    }
    if (Btn(520, 640, 240, 46, "Voltar", 0) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
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
    Fundo(); Painel(40, 24, 1200, 672, .03f, PANEL); TxtCS("HISTÓRICO", 640, 38, 40, OURO);
    if (Btn(80, 100, 270, 46, "Ordem alfabética", hOrd == 1 ? 0 : 1)) ordena(0);
    if (Btn(365, 100, 320, 46, "Pontuação decrescente", hOrd == 2 ? 0 : 1)) ordena(1);
    if (hOrd) Txt(TextFormat("%d partidas · ordenação %.6f s", hv.tamanho, hTempo), 710, 112, 20, MUTE);
    else Txt("Escolha como ordenar.", 710, 112, 20, MUTE);
    Txt("#", 100, 166, 16, MUTE); Txt("JOGADOR", 190, 166, 16, MUTE); Txt("PONTOS", 1060, 166, 16, MUTE);
    if (hvOk) {
        hScroll -= (int)GetMouseWheelMove() * 3; if (hScroll > hv.tamanho - 11) hScroll = hv.tamanho - 11; if (hScroll < 0) hScroll = 0;
        for (int i = 0; i < 11 && hScroll + i < hv.tamanho; i++) {
            NovoHistorico h; obtemContig(&hv, hScroll + i, &h); int idx = hScroll + i; float y = 190 + i * 40;
            if (i & 1) Rr(60, y, 1160, 36, .4f, SUB);
            if (hOrd == 2 && idx < 3) { Color m[3] = {{255,205,60,255},{200,205,215,255},{205,127,50,255}}; DrawCircle(112, (int)y + 18, 16, INK); DrawCircle(112, (int)y + 18, 14, m[idx]); TxtC(TextFormat("%d", idx + 1), 112, y + 8, 20, INK); }
            else Txt(TextFormat("%d", idx + 1), 100, y + 8, 20, MUTE);
            Txt(h.nome, 190, y + 7, 22, INK); Txt(TextFormat("%d", h.pontuacao), 1060, y + 7, 22, (Color){200,100,20,255});
        }
        Txt("Role o mouse para ver mais", 900, 655, 16, MUTE);
    }
    if (Btn(80, 640, 200, 46, "Voltar", 0) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}
static void telaNome(void) {
    Fundo(); Painel(340, 200, 600, 300, .06f, PANEL); TxtCS("Qual é o seu nome, atendente?", 640, 228, 30, OURO);
    for (int c = GetCharPressed(); c; c = GetCharPressed()) if (c >= 32 && c < 127 && nomeLen < 20) { nome[nomeLen++] = (char)c; nome[nomeLen] = 0; }
    if (IsKeyPressed(KEY_BACKSPACE) && nomeLen) nome[--nomeLen] = 0;
    RrC(400, 300, 480, 60, .3f, (Color){255,252,242,255}, INK); Txt(nome, 424, 316, 30, INK);
    if (((int)(GetTime() * 2)) & 1) DrawRectangle(426 + (int)TW(nome, 30), 314, 3, 34, ACC);
    if ((Btn(520, 400, 240, 54, "Começar!", 3) || IsKeyPressed(KEY_ENTER)) && nomeLen) novaPartida();
    if (IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}

static void Chip(char s, float x, float y, int feito) {      /* ficha redonda de ingrediente */
    DrawCircle((int)x, (int)y, 24, INK);
    DrawCircle((int)x, (int)y, 21, feito ? (Color){150,225,160,255} : (Color){255,250,236,255}); Icone(s, x, y, 32);
}

static void telaJogo(void) {
    Fundo(); Cliente cl; frenteFila(&jogo.clientes, &cl); float dt = GetFrameTime();
    float sx = shake > 0 ? GetRandomValue(-4, 4) : 0; shake -= dt;
    for (int i = 0; i < 12; i++) if (IsKeyPressed(KEY_A + (ING[i].s - 'a')) && jogo.status == JOGO_ANDAMENTO)
        acao((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) ? 1 : 0, ING[i].s);
    if (jogo.status == JOGO_ANDAMENTO && IsKeyPressed(KEY_BACKSPACE)) acao(2, 0);
    if (jogo.status == JOGO_ANDAMENTO && IsKeyPressed(KEY_ESCAPE)) { jogoAbandona(&jogo); encerra(); }
    if (jogo.status != JOGO_ANDAMENTO) return;
    frenteFila(&jogo.clientes, &cl);
    float pf = (float)jogo.paciencia / jogo.pacienciaMax;
    Color pc = pf > .5f ? (Color){40,200,60,255} : pf > .25f ? (Color){250,170,40,255} : (Color){230,70,70,255};

    /* ---- HUD ---- */
    DrawRectangle(0, 0, W, 64, INK); DrawRectangle(0, 64, W, 3, (Color){28,12,8,255});
    Txt("DADO BURGER", 20, 14, 36, OURO);
    Txt(TextFormat("Turno %d", jogo.turno), 381, 17, 30, CREAM);
    Txt(TextFormat("Atendidos %d/%d", jogo.atendidos, jogo.totalClientes), 560, 17, 30, CREAM);
    Txt(TextFormat("Pontos %d", jogo.pontuacao), 840, 17, 30, CREAM);
    TxtR("Esc: abandonar", 1260, 22, 22, OURO);

    /* ---- painel esquerdo: cliente, receita, fila ---- */
    Painel(14, 78, 334, 566, .03f, PANEL);
    Rosto(109, 154 + 2 * sinf((float)GetTime() * 4), 50, hnome(cl.nome), pf);
    Txt(cl.nome, 173, 136, 28, INK); Txt(TextFormat("Pedido #%d", cl.pedido.cod), 173, 168, 20, MUTE);
    RrC(30, 241, 300, 20, .4f, (Color){232,210,168,255}, INK);
    if (pf > 0) Rr(30, 241, 300 * pf < 8 ? 8 : 300 * pf, 20, .4f, pc);
    TxtC(TextFormat("Paciência %d/%d", jogo.paciencia, jogo.pacienciaMax), 180, 267, 20, INK);

    char rec[40], pil[40]; int nr = leReceita(rec), np = lePilha(pil);
    int ok = 1; for (int i = 0; i < np; i++) if (i >= nr || rec[i] != pil[i]) { ok = 0; break; }
    Txt("Receita:", 29, 308, 22, INK);
    for (int i = 0; i < nr; i++) Chip(rec[i], 57 + (i % 6) * 48, 363 + (i / 6) * 48, i < np && ok);
    int rows = (nr + 5) / 6, yF = 470 + (rows > 2 ? (rows - 2) * 48 : 0), maxf = rows > 2 ? 5 : 10;
    Txt(TextFormat("Fila: %d cliente(s)", tamanhoFila(&jogo.clientes)), 29, yF, 22, INK);
    int k = 0; for (Nodo *n = jogo.clientes.frente; n && k < maxf; n = n->prox, k++)
        Rosto(55 + (k % 5) * 58, yF + 58 + (k / 5) * 61, 25, hnome(n->cliente.nome), k == 0 ? pf : 1);

    /* ---- palco: pedido x seu hamburguer ---- */
    Painel(360 + sx, 78, 484, 566, .03f, PANEL);
    TxtC("PEDIDO", 481 + sx, 92, 26, INK); TxtC("SEU HAMBÚRGUER", 721 + sx, 92, 26, INK);
    DesenhaPrato(rec, nr, 481 + sx, 560, NULL, 0, -1); DesenhaPrato(pil, np, 721 + sx, 560, rec, nr, GetTime() - tPush);
    if (np && pil[np-1] == 'z' && !ok) { RrC(621 + sx, 122, 200, 30, .5f, (Color){255,190,180,255}, (Color){180,70,70,255}); TxtC("Errado! Remova o pão", 721 + sx, 129, 16, INK); }

    /* ---- barra inferior ---- */
    if (Btn(358, 650, 224, 54, "Remover (Bksp)", 2)) acao(2, 0);
    if (jogo.status != JOGO_ANDAMENTO) return;
    Txt("Letra: adicionar", 600, 662, 18, INK);
    Txt("Shift+letra / clique em COZINHAR: preparar", 600 + TW("Letra: adicionar", 18) + 26, 662, 18, INK);

    /* ---- prateleira de ingredientes ---- */
    Painel(856, 78, 412, 566, .03f, PANEL); char prox = (ok && np < nr) ? rec[np] : 0; Vector2 m = GetMousePosition();
    for (int i = 0; i < 12; i++) {
        Ing *g = &ING[i]; Rectangle r = {864 + (i % 3) * 134.f, 89 + (i / 3) * 137.f, 128, 129};
        int qtd = g->prep ? consultaEstoque(&jogo.estoque, g->ing) : -1; int hot = CheckCollisionPointRec(m, r);
        float ty = r.y + (hot ? -2 : 0), cx = r.x + 64;
        if (g->s == prox) Rr(r.x - 6, ty - 6, 140, 141, .12f, Fade(OURO, .55f + .45f * sinf((float)GetTime() * 6)));
        RrC(r.x, ty, 128, 129, .1f, hot ? (Color){255,253,244,255} : CARDC, INK);
        Txt(TextFormat("[%c]", g->s), r.x + 8, ty + 6, 16, INK);
        Icone(g->s, cx, ty + 44, 60);
        TxtC(g->nome, cx, ty + 78, 18, INK);
        if (!g->prep) TxtC("infinito", cx, ty + 100, 16, (Color){30,150,90,255});
        else {
            int v = g->perec ? consultaValidade(&jogo.estoque, g->ing, jogo.turno) : -1;
            if (v >= 0) TxtR(TextFormat("vence em %d", v), r.x + 120, ty + 8, 12, v <= 1 ? BAD : MUTE);
            Rectangle pb = {r.x + 64, r.y + 102, 58, 22}; int hp = CheckCollisionPointRec(m, pb);
            Txt(TextFormat("x%d", qtd), r.x + 9, ty + 100, 18, qtd > 0 ? (Color){30,150,90,255} : BAD);
            RrB(pb.x, pb.y, pb.width, pb.height, .3f, hp ? Lt((Color){240,196,130,255}, .3f) : (Color){240,196,130,255}, INK, 2);
            TxtC("COZINHAR", pb.x + 29, pb.y + 5, 11, INK);
            if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && fadeA < .2f && hp) { acao(1, g->s); if (jogo.status != JOGO_ANDAMENTO) return; continue; }
        }
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && fadeA < .2f) { acao(0, g->s); if (jogo.status != JOGO_ANDAMENTO) return; }
        if (hot && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) { acao(1, g->s); if (jogo.status != JOGO_ANDAMENTO) return; }
    }

    /* ---- efeitos ---- */
    if (flash > 0) { DrawRectangle(0, 0, W, H, Fade(WHITE, flash)); flash -= dt; }
    if (toastT > 0) { toastT -= dt; float a = toastT * 3 > 1 ? 1 : toastT * 3, yy = 74 - (1 - a) * 24;
        RrC(430, yy, 420, 38, .5f, Fade(toastC, a), Fade(INK, a)); TxtC(toast, 640, yy + 9, 18, Fade(WHITE, a)); }
}

static void telaFim(void) {
    Fundo(); Painel(240, 40, 800, 640, .03f, PANEL); int vit = fim.st == JOGO_VITORIA;
    TxtCS(vit ? "VITÓRIA!" : fim.st == JOGO_DERROTA ? "O cliente desistiu..." : "Partida abandonada", 640, 62, 56, vit ? (Color){70,210,120,255} : (Color){240,100,90,255});
    TxtC(nome, 640, 132, 28, MUTE);
    int ganhos = fim.total ? (fim.atend * 3 + fim.total - 1) / fim.total : 0;
    for (int i = 0; i < 3; i++) { if (i < ganhos) Icone('z', 540 + i * 100, 205, 64); else { DrawCircle(540 + i * 100, 205, 28, INK); DrawCircle(540 + i * 100, 205, 25, SUB); } }
    TxtC(TextFormat("%d pontos  ·  %d/%d clientes  ·  %d turnos", fim.pont, fim.atend, fim.total, fim.turno - 1), 640, 262, 24, INK);
    long tot = fim.hits + fim.misses; float cx[3] = {262, 517, 772}; Color hd = {200,100,20,255};
    for (int i = 0; i < 3; i++) RrC(cx[i], 320, 245, 190, .08f, SUB, INK);
    Txt("AVL · carga", cx[0] + 16, 332, 20, hd);
    Txt(TextFormat("%d nós", carga.nos), cx[0] + 16, 368, 17, INK); Txt(TextFormat("altura %d", carga.altura), cx[0] + 16, 394, 17, INK);
    Txt(TextFormat("%d rotações", carga.rotacoes), cx[0] + 16, 420, 17, INK); Txt(TextFormat("%.3f s", carga.tempoCarga), cx[0] + 16, 446, 17, INK);
    Txt("AVL · buscas", cx[1] + 16, 332, 20, hd);
    Txt(TextFormat("%ld buscas", fim.buscas), cx[1] + 16, 368, 17, INK); Txt(TextFormat("%ld comparações", fim.comps), cx[1] + 16, 394, 17, INK);
    Txt(TextFormat("média %.2f", fim.buscas ? (double)fim.comps / fim.buscas : 0.0), cx[1] + 16, 420, 17, INK); Txt(TextFormat("%.6f s", fim.tBusca), cx[1] + 16, 446, 17, INK);
    Txt("Tabela hash", cx[2] + 16, 332, 20, hd);
    Txt(TextFormat("hits %ld · misses %ld", fim.hits, fim.misses), cx[2] + 16, 368, 17, INK); Txt(TextFormat("acerto %.1f%%", tot ? 100.0 * fim.hits / tot : 0.0), cx[2] + 16, 394, 17, INK);
    Txt(TextFormat("carga %.2f (%d/%d)", fim.m ? (double)fim.n / fim.m : 0.0, fim.n, fim.m), cx[2] + 16, 420, 17, INK); Txt(TextFormat("colisões %ld", fim.col), cx[2] + 16, 446, 17, INK);
    Txt("Relatório completo em medicoes.log", 262, 484, 14, MUTE);
    if (Btn(300, 560, 300, 58, "Jogar de novo", 3) || IsKeyPressed(KEY_ENTER)) novaPartida();
    if (Btn(680, 560, 300, 58, "Menu", 1) || IsKeyPressed(KEY_ESCAPE)) irPara(T_MENU);
}

static Texture2D carrega(const char *p) {        /* carrega PNG opcional com filtro suave */
    Texture2D t = {0}; if (FileExists(p)) { t = LoadTexture(p); GenTextureMipmaps(&t); SetTextureFilter(t, TEXTURE_FILTER_TRILINEAR); } return t; }
/* ---------------------------------- main ---------------------------------- */
int main(int argc, char *argv[]) {
    if (argc < 3) { fprintf(stderr, "Uso: %s <pedidos> <historico>\n", argv[0]); return 1; }
    srand((unsigned)time(NULL)); arqHist = argv[2]; raiz = NULL; inicializaListaContig(&hist); avlZerarMetricas();
    Timer t; timer_iniciar(&t); int lidos = carregarPedidos(argv[1], &raiz); double tc = timer_parar(&t);   /* mesma carga do modo terminal */
    if (lidos < 0) {
        if (lidos == -3)
            fprintf(stderr, "Erro: memoria insuficiente para carregar os pedidos.\n");
        else
            fprintf(stderr, "Erro ao carregar pedidos.\n");
        // libera os pedidos carregados antes da falha
        raiz = avlDestruir(raiz);
        destroiListaContig(&hist);
        return 1;
    }
    int nh = carregarHistorico(argv[2], &hist); Contador mc = avlObterMetricas();
    carga = (MetricasCarga){ mc.nosInserido, avlAltura(raiz), mc.rotacoes, tc, nh }; metricasLogCarga(LOG, &carga);

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT); InitWindow(W, H, "Dado Burger"); SetTargetFPS(60); SetExitKey(KEY_NULL);

    /* fonte pixelada opcional (assets/font.ttf), com acentos do Latin-1 */
    if (FileExists("assets/font.ttf")) {
        int cp[224]; for (int i = 0; i < 224; i++) cp[i] = 32 + i;
        fonte = LoadFontEx("assets/font.ttf", 40, cp, 224);
        if (fonte.texture.id) { fonteOk = 1; SetTextureFilter(fonte.texture, TEXTURE_FILTER_POINT); }
    }
    if (!fonteOk) fonte = GetFontDefault();

    for (int i = 0; i < 12; i++) { char p[64]; snprintf(p, sizeof p, "assets/ico_%c.png", ING[i].s); ING[i].tex = carrega(p);
        snprintf(p, sizeof p, "assets/lay_%c.png", ING[i].s); ING[i].lay = carrega(p); }
    baseTex = carrega("assets/lay_base.png");
    while (!quit && !WindowShouldClose()) {
        float dt = GetFrameTime();
        BeginDrawing(); ClearBackground(PANEL);
        switch (tela) { case T_MENU: telaMenu(); break; case T_COMO: telaComo(); break; case T_HIST: telaHist(); break;
                        case T_NOME: telaNome(); break; case T_JOGO: telaJogo(); break; case T_FIM: telaFim(); break; }
        for (int i = 0; i < 120; i++) if (part[i].life > 0) {            /* confete */
            part[i].life -= dt; part[i].v.y += 900 * dt; part[i].p.x += part[i].v.x * dt; part[i].p.y += part[i].v.y * dt;
            DrawRectangle((int)part[i].p.x, (int)part[i].p.y, 8, 8, part[i].c); }
        desenhaPops(dt);
        if (fading) { fadeA += dt * 4; if (fadeA >= 1) { tela = proxT; fading = 0; fadeA = 1; if (tela == T_NOME) { nome[0] = 0; nomeLen = 0; } } }
        else if (fadeA > 0) fadeA -= dt * 4;
        if (fadeA > 0) DrawRectangle(0, 0, W, H, Fade(INK, fadeA));
        EndDrawing();
    }
    if (tela == T_JOGO && jogo.status == JOGO_ANDAMENTO) destroiJogo(&jogo);
    if (fonteOk) UnloadFont(fonte);
    CloseWindow(); if (hvOk) destroiListaContig(&hv);
    avlDestruir(raiz); destroiListaContig(&hist); return 0;
}
