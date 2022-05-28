use core::{future::Future, time::Duration};

#[cfg(target_arch = "wasm32")]
mod web_assembly {
    use core::{
        future::Future,
        marker::{PhantomData, Unpin},
        mem::ManuallyDrop,
        ops::Drop,
        pin::Pin,
        ptr,
        task::{Context, Poll},
        time::Duration,
    };
    use futures::{
        future::{self, AbortHandle, Abortable},
        stream::Aborted,
        FutureExt,
    };
    use wasm_mt::utils as wasm_mt_utils;

    pub struct AbortableFuture<Fut: Future> {
        future: Abortable<Fut>,
        handler: AbortHandle,
    }

    pub struct Timeout<Fut: Future> {
        abortable_future: AbortableFuture<Fut>,
        timeout_deadline: Duration,
    }
    impl<Fut: Future> Timeout<Fut> {
        pub fn new(abortable_future: Fut, timeout_deadline: Duration) -> Self {
            let (abortable_future, abortable_handler) = future::abortable(abortable_future);
            let abortable_future = AbortableFuture {
                future: abortable_future,
                handler: abortable_handler,
            };

            Self {
                abortable_future,
                timeout_deadline,
            }
        }

        pub async fn run(self) {
            let Timeout {
                abortable_future: AbortableFuture { future, handler },
                timeout_deadline,
            } = self;

            futures::select! {
                _ = future.fuse() => {},
                _ = wasm_mt_utils::sleep(timeout_deadline.as_millis() as u32).fuse() => {
                    handler.abort();
                }
            }
        }
    }

    // This is an internal helper struct used by `pin_project!`.
    pub struct AlwaysUnpin<T: ?Sized>(PhantomData<T>);

    impl<T: ?Sized> Unpin for AlwaysUnpin<T> {}

    // This is an internal helper used to ensure a value is dropped.
    pub struct UnsafeDropInPlaceGuard<T: ?Sized>(*mut T);

    impl<T: ?Sized> Drop for UnsafeDropInPlaceGuard<T> {
        fn drop(&mut self) {
            unsafe {
                ptr::drop_in_place(self.0);
            }
        }
    }

    // This is an internal helper used to ensure a value is overwritten without
    // its destructor being called.
    pub struct UnsafeOverwriteGuard<T> {
        target: *mut T,
        value: ManuallyDrop<T>,
    }

    impl<T> Drop for UnsafeOverwriteGuard<T> {
        fn drop(&mut self) {
            unsafe {
                ptr::write(self.target, ptr::read(&*self.value));
            }
        }
    }

    pub struct Projection<'p, Fut: Future>
    where
        Timeout<Fut>: 'p,
    {
        abortable_future: Pin<&'p mut AbortableFuture<Fut>>,
        _timeout_deadline: Pin<&'p mut Duration>,
    }
    impl<Fut: Future> Timeout<Fut> {
        pub fn project<'p>(self: Pin<&'p mut Self>) -> Projection<'p, Fut> {
            unsafe {
                let Self {
                    abortable_future,
                    timeout_deadline,
                } = self.get_unchecked_mut();

                Projection {
                    abortable_future: Pin::new_unchecked(abortable_future),
                    _timeout_deadline: Pin::new_unchecked(timeout_deadline),
                }
            }
        }
    }

    impl<Fut: Future> Future for Timeout<Fut> {
        type Output = Option<Result<<Fut as futures::Future>::Output, Aborted>>;

        fn poll(self: Pin<&mut Self>, ctx: &mut Context) -> Poll<Self::Output> {
            let this = self.project();

            let future_is_aborted = this.abortable_future.future.is_aborted();

            let future_poll = unsafe {
                Pin::new_unchecked(&mut this.abortable_future.map_unchecked_mut(|s| &mut s.future))
            }
            .poll(ctx);

            if let Poll::Ready(output) = future_poll {
                return Poll::Ready(Some(output));
            } else if future_is_aborted {
                return Poll::Ready(None);
            }

            Poll::Pending
        }
    }
}

pub async fn timeout<F>(duration: Duration, future: F) -> anyhow::Result<()>
where
    F: Future<Output = ()> + Send + 'static,
{
    #[cfg(not(target_arch = "wasm32"))]
    {
        if let Err(e) = tokio::time::timeout(duration, future).await {
            anyhow::bail!("timeout reached after {}", e)
        }
    }

    #[cfg(target_arch = "wasm32")]
    {
        web_assembly::Timeout::new(future, duration).run().await;
    }

    Ok(())
}
