.class public Lcom/robtopx/geometryjump/ClipboardHelper;
.super Ljava/lang/Object;

.method public static copyToClipboard(Ljava/lang/String;)V
    .locals 3

    # v0 = GeometryJump.getInstance()
    invoke-static {}, Lcom/robtopx/geometryjump/GeometryJump;->getInstance()Lcom/robtopx/geometryjump/GeometryJump;
    move-result-object v0

    new-instance v1, Lcom/robtopx/geometryjump/ClipboardHelper$1;
    invoke-direct {v1, p0}, Lcom/robtopx/geometryjump/ClipboardHelper$1;-><init>(Ljava/lang/String;)V

    # runOnUiThread(runnable)
    invoke-virtual {v0, v1}, Lcom/robtopx/geometryjump/GeometryJump;->runOnUiThread(Ljava/lang/Runnable;)V

    return-void
.end method