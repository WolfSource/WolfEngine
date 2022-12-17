#![allow(clippy::default_trait_access)]
#![allow(clippy::derive_partial_eq_without_eq)]
#![allow(clippy::doc_markdown)]
#![allow(clippy::future_not_send)]
#![allow(clippy::missing_const_for_fn)]
#![allow(clippy::missing_errors_doc)]
#![allow(clippy::must_use_candidate)]
#![allow(clippy::return_self_not_must_use)]
#![allow(clippy::similar_names)]
#![allow(clippy::use_self)]
#![allow(clippy::wildcard_imports)]
#![allow(missing_debug_implementations)]
#![allow(unused_qualifications)]

use super::raft_imp::{self, MemRaft};
use std::collections::HashSet;
use std::sync::Arc;
use tonic::{Request, Response, Status};
use wolf_raft::raft_server::{Raft, RaftServer};
use wolf_raft::{
    raft_append_entries_res, raft_bootstrap_res, raft_get_metrics_res, raft_install_snapshot_res,
    raft_vote_res, ErrorCode, RaftAppendEntriesReq, RaftAppendEntriesRes, RaftBootstrapOkRes,
    RaftBootstrapReq, RaftBootstrapRes, RaftErrorRes, RaftGetMetricsOkRes, RaftGetMetricsReq,
    RaftGetMetricsRes, RaftInstallSnapshotReq, RaftInstallSnapshotRes, RaftVoteReq, RaftVoteRes,
};
const BASE_ERROR_CODE_RAFT: i32 = 7000;

pub mod wolf_raft {
    tonic::include_proto!("wolf.raft");
}

pub struct Srv {
    raft_node: Arc<MemRaft>,
}

impl Default for Srv {
    fn default() -> Self {
        let cluster_name = "wolf";
        let node_id = 0;
        Self {
            raft_node: Arc::new(raft_imp::new(cluster_name, node_id)),
        }
    }
}

impl Srv {
    fn new(p_cluster_name: &str, p_node_id: u64) -> Self {
        Self {
            raft_node: Arc::new(raft_imp::new(p_cluster_name, p_node_id)),
        }
    }
}

#[tonic::async_trait]
impl Raft for Srv {
    async fn bootstrap(
        &self,
        p_request: Request<RaftBootstrapReq>,
    ) -> Result<Response<RaftBootstrapRes>, Status> {
        // info!(
        //     "received a bootstrap request from {:?} : {:?}",
        //     p_request.remote_addr(),
        //     p_request
        // );

        // get inner message or request
        let inner = p_request.into_inner();

        // initialize raft node with the following members
        let mut members = HashSet::new();
        for i in 0..inner.number_of_nodes {
            let _r = members.insert(i);
        }
        self.raft_node
            .initialize(members)
            .await
            .map(|_| {
                // create Ok response
                let ok_res = RaftBootstrapOkRes {
                    msg_id: inner.msg_id.clone(),
                };
                let ok = raft_bootstrap_res::Response::OkRes(ok_res);
                let resp = RaftBootstrapRes { response: Some(ok) };
                Response::new(resp)
            })
            .or_else(|e| {
                // create Error response
                let err_res = RaftErrorRes {
                    code: BASE_ERROR_CODE_RAFT + (ErrorCode::RaftBootstrapFailed as i32),
                    msg_id: inner.msg_id,
                    msg: format!("could not initialize raft node because {e:?}"),
                };
                let err = raft_bootstrap_res::Response::ErrorRes(err_res);
                let resp = RaftBootstrapRes {
                    response: Some(err),
                };
                Ok(Response::new(resp))
            })
    }

    async fn append_entries(
        &self,
        p_request: Request<RaftAppendEntriesReq>,
    ) -> Result<Response<RaftAppendEntriesRes>, Status> {
        // info!(
        //     "received a append_entries request from {:?} : {:?}",
        //     p_request.remote_addr(),
        //     p_request
        // );

        // get inner message of the request
        let inner_req = p_request.into_inner();
        let req_result =
            super::raft_converter::grpc_append_entries_req_to_raft_append_entries_req(&inner_req);
        match req_result {
            Ok(req) => {
                let res = self.raft_node.append_entries(req).await;
                let resp = match res {
                    Ok(app_res) => {
                        //convert AppendEntriesResponse to RaftAppendEntriesOkRes
                        let ok_resp =
                        super::raft_converter::raft_append_entries_response_to_grpc_append_entries_ok_res(
                                inner_req.msg_id,
                                &app_res,
                            );
                        let ok = raft_append_entries_res::Response::OkRes(ok_resp);
                        RaftAppendEntriesRes { response: Some(ok) }
                    }
                    Err(e) => {
                        // create a custome error response
                        let err_res = RaftErrorRes {
                            code: BASE_ERROR_CODE_RAFT
                                + (ErrorCode::RaftAppendEntriesFailed as i32),
                            msg_id: inner_req.msg_id,
                            msg: format!(
                                "could not get response from raft::append_entries because {e:?}"
                            ),
                        };
                        let err = raft_append_entries_res::Response::ErrorRes(err_res);
                        RaftAppendEntriesRes {
                            response: Some(err),
                        }
                    }
                };
                Ok(Response::new(resp))
            }
            Err(e) => {
                // create a custome error response
                let err_res = RaftErrorRes {
                    code: BASE_ERROR_CODE_RAFT
                        + (ErrorCode::RaftAppendEntriesJsonOfReqIsNotValid as i32),
                    msg_id: inner_req.msg_id,
                    msg: format!("could not deserialize append_entries request because {e:?}"),
                };
                let err = raft_append_entries_res::Response::ErrorRes(err_res);
                let resp = RaftAppendEntriesRes {
                    response: Some(err),
                };
                Ok(Response::new(resp))
            }
        }
    }

    async fn install_snapshot(
        &self,
        p_request: Request<RaftInstallSnapshotReq>,
    ) -> Result<Response<RaftInstallSnapshotRes>, Status> {
        // info!(
        //     "received a install_snapshot request from {:?} : {:?}",
        //     p_request.remote_addr(),
        //     p_request
        // );

        // get inner message of the request
        let inner_req = p_request.into_inner();
        let req = super::raft_converter::grpc_install_snapshot_req_to_raft_install_snapshot_req(
            &inner_req,
        );
        let res = self.raft_node.install_snapshot(req).await;
        match res {
            Ok(r) => {
                let ok_resp = super::raft_converter::raft_install_snapshot_res_to_grpc_install_snapshot_ok_res(
                    inner_req.msg_id,
                    &r,
                );
                let ok = raft_install_snapshot_res::Response::OkRes(ok_resp);
                let resp = RaftInstallSnapshotRes { response: Some(ok) };
                Ok(Response::new(resp))
            }
            Err(e) => {
                // create a custome error response
                let err_res = RaftErrorRes {
                    code: BASE_ERROR_CODE_RAFT + (ErrorCode::RaftInstallSnapshotFailed as i32),
                    msg_id: inner_req.msg_id,
                    msg: format!(
                        "could not get response from raft::install_snapshot because {e:?}"
                    ),
                };
                let err = raft_install_snapshot_res::Response::ErrorRes(err_res);
                let resp = RaftInstallSnapshotRes {
                    response: Some(err),
                };
                Ok(Response::new(resp))
            }
        }
    }

    async fn vote(&self, p_request: Request<RaftVoteReq>) -> Result<Response<RaftVoteRes>, Status> {
        // info!(
        //     "received a vote request from {:?} : {:?}",
        //     p_request.remote_addr(),
        //     p_request
        // );

        // get inner message of the request
        let inner_req = p_request.into_inner();
        let req = super::raft_converter::grpc_vote_req_to_raft_vote_req(&inner_req);
        let res = self.raft_node.vote(req).await;
        match res {
            Ok(r) => {
                let ok_resp =
                    super::raft_converter::raft_vote_res_to_grpc_vote_ok_res(inner_req.msg_id, &r);
                let ok = raft_vote_res::Response::OkRes(ok_resp);
                let resp = RaftVoteRes { response: Some(ok) };
                Ok(Response::new(resp))
            }
            Err(e) => {
                // create a custome error response
                let err_res = RaftErrorRes {
                    code: BASE_ERROR_CODE_RAFT + (ErrorCode::RaftVoteFailed as i32),
                    msg_id: inner_req.msg_id,
                    msg: format!("could not get response from raft::vote because {e:?}"),
                };
                let err = raft_vote_res::Response::ErrorRes(err_res);
                let resp = RaftVoteRes {
                    response: Some(err),
                };
                Ok(Response::new(resp))
            }
        }
    }

    async fn get_metrics(
        &self,
        p_request: Request<RaftGetMetricsReq>,
    ) -> Result<Response<RaftGetMetricsRes>, Status> {
        // info!(
        //     "received a get metrics request from {:?} : {:?}",
        //     p_request.remote_addr(),
        //     p_request
        // );

        // get inner message of the request
        let inner_req = p_request.into_inner();
        let metrics = self.raft_node.metrics().borrow().clone();

        serde_json::to_string(&metrics)
            .map(|json| {
                let ok_resp = RaftGetMetricsOkRes {
                    msg_id: inner_req.msg_id.clone(),
                    metrics: json,
                };
                let ok = raft_get_metrics_res::Response::OkRes(ok_resp);
                let resp = RaftGetMetricsRes { response: Some(ok) };
                Response::new(resp)
            })
            .or_else(|e| {
                // create a custome error response
                let err_res = RaftErrorRes {
                    code: BASE_ERROR_CODE_RAFT + (ErrorCode::RaftGetMetricsFailed as i32),
                    msg_id: inner_req.msg_id,
                    msg: format!("could not get json response from raft::metrics because {e:?}"),
                };
                let err = raft_get_metrics_res::Response::ErrorRes(err_res);
                let resp = RaftGetMetricsRes {
                    response: Some(err),
                };
                Ok(Response::new(resp))
            })
    }
}

#[must_use]
pub fn get_service(p_cluster_name: &str, p_node_id: u64) -> RaftServer<Srv> {
    let srv = Srv::new(p_cluster_name, p_node_id);
    let compress_type = tonic::codec::CompressionEncoding::Gzip;
    RaftServer::new(srv)
        .send_compressed(compress_type)
        .accept_compressed(compress_type)
}
