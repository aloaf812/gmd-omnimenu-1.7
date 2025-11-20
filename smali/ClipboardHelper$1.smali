.class public Lcom/robtopx/geometryjump/ClipboardHelper$1;
.super Ljava/lang/Object;
.implements Ljava/lang/Runnable;

.field private final text:Ljava/lang/String;

.method public constructor <init>(Ljava/lang/String;)V
    .locals 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V
    iput-object p1, p0, Lcom/robtopx/geometryjump/ClipboardHelper$1;->text:Ljava/lang/String;
    return-void
.end method


.method public run()V
    .locals 4

    # v0 = GeometryJump.getInstance()
    invoke-static {}, Lcom/robtopx/geometryjump/GeometryJump;->getInstance()Lcom/robtopx/geometryjump/GeometryJump;
    move-result-object v0

    # v1 = Context.CLIPBOARD_SERVICE
    const-string v1, "clipboard"

    # clipboard = activity.getSystemService(...)
    invoke-virtual {v0, v1}, Lcom/robtopx/geometryjump/GeometryJump;->getSystemService(Ljava/lang/String;)Ljava/lang/Object;
    move-result-object v1

    check-cast v1, Landroid/text/ClipboardManager;

    iget-object v2, p0, Lcom/robtopx/geometryjump/ClipboardHelper$1;->text:Ljava/lang/String;

    # clipboard.setText(text)
    invoke-virtual {v1, v2}, Landroid/text/ClipboardManager;->setText(Ljava/lang/CharSequence;)V

    return-void
.end method
