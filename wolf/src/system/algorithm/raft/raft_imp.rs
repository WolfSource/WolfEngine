use crate::{
    error::WError,
    stream::grpc,
    system::algorithm::raft::{
        raft_converter, raft_srv::wolf_raft, raft_srv::wolf_raft::raft_client::RaftClient,
    },
};
use async_raft::{
    raft::{self, InstallSnapshotRequest, InstallSnapshotResponse, VoteRequest, VoteResponse},
    Config, NodeId, RaftNetwork,
};
use async_trait::async_trait;
use memstore::{ClientRequest, ClientResponse, MemStore};
use std::sync::Arc;
use uuid::Uuid;

pub type MemRaft = async_raft::Raft<ClientRequest, ClientResponse, RaftRouter, MemStore>;
const BASE_PORT: u64 = 7777;

#[derive(Debug)]
pub struct RaftRouter {}

impl Default for RaftRouter {
    fn default() -> Self {
        Self::new()
    }
}

impl RaftRouter {
    /// Create a new `RaftRouter` instance.
    #[must_use]
    pub const fn new() -> Self {
        Self {}
    }
}

#[async_trait]
impl RaftNetwork<ClientRequest> for RaftRouter {
    /// Send an `AppendEntries` RPC to the target Raft node
    async fn append_entries(
        &self,
        p_target_node: NodeId,
        p_rpc: raft::AppendEntriesRequest<ClientRequest>,
    ) -> Result<raft::AppendEntriesResponse, WError> {
        let uuid = Uuid::new_v5(&Uuid::NAMESPACE_X500, b"wolf_raft_append_entries");
        let res_append = raft_converter::raft_append_entries_req_to_grpc_append_entries_req(
            uuid.to_string(),
            &p_rpc,
        );
        match res_append {
            Ok(raft_req) => {
                //create a channel for grpc
                let uri = format!("http://localhost:{}", BASE_PORT + p_target_node);
                match grpc::create_channel(uri).await {
                    Ok(c) => {
                        //call request with channel
                        let raft_client = RaftClient::new(c);
                        let compress_type = tonic::codec::CompressionEncoding::Gzip;
                        let mut client = raft_client
                            .send_compressed(compress_type)
                            .accept_compressed(compress_type);
                        match client.append_entries(raft_req).await {
                            Ok(r) => {
                                match r.into_inner().response {
                                    Some(s) => {
                                        use wolf_raft::raft_append_entries_res::Response;
                                        if let Response::OkRes(ok) = s {
                                            //create AppendEntriesResponse from RaftAppendEntriesOkRes
                                            let ret = raft_converter::grpc_append_entries_ok_res_to_raft_append_entries_res(&ok);
                                            Ok(ret)
                                        } else if let Response::ErrorRes(_e) = s {
                                            Err(WError::SystemRaftAppendEntriesErrorResponse)
                                        } else {
                                            Err(WError::SystemRaftAppendEntriesErrorResponse)
                                        }
                                    }
                                    None => Err(WError::SystemRaftAppendEntriesNoResponse),
                                }
                            }
                            Err(e) => Err(WError::SystemRaftAppendEntriesFailed),
                        }
                    }
                    Err(e) => Err(WError::SystemRaftAppendEntriesCreateChannelFailed),
                }
            }
            Err(e) => Err(WError::SystemRaftAppendEntriesFailed),
        }
    }

    /// Send an `InstallSnapshot` RPC to the target Raft node
    async fn install_snapshot(
        &self,
        p_target_node: u64,
        p_rpc: InstallSnapshotRequest,
    ) -> Result<InstallSnapshotResponse> {
        const TRACE: &str = "raft_imp::install_snapshot";

        //create a channel for grpc
        let uri = format!("http://localhost:{}", BASE_PORT + p_target_node);
        match grpc::create_channel(uri).await {
            Ok(c) => {
                //call request with channel
                let uuid = Uuid::new_v5(&Uuid::NAMESPACE_X500, b"wolf_raft_install_snapshot");
                let msg_id = uuid.to_string();
                let rpc_req =
                    raft_converter::raft_install_snapshot_req_to_grpc_install_snapshot_req(
                        msg_id, &p_rpc,
                    );
                let compress_type = tonic::codec::CompressionEncoding::Gzip;
                let mut client = RaftClient::new(c)
                    .send_compressed(compress_type)
                    .accept_compressed(compress_type);
                match client.install_snapshot(rpc_req).await {
                    Ok(r) => match r.into_inner().response {
                        Some(s) => {
                            use wolf_raft::raft_install_snapshot_res::Response;
                            if let Response::OkRes(ok) = s {
                                let response = raft_converter::grpc_install_snapshot_ok_res_to_raft_install_snapshot_res(&ok);
                                Ok(response)
                            } else if let Response::ErrorRes(e) = s {
                                bail!(
                                        "InstallSnapshotResponse for node {} contains Error {:?}. Trace: {}",
                                        p_target_node,
                                        e,
                                        TRACE
                                    )
                            } else {
                                bail!(
                                        "InstallSnapshotResponse for node {} contains Unknown error. Trace: {}",
                                        p_target_node,
                                        TRACE
                                    )
                            }
                        }
                        None => {
                            bail!(
                                    "inner message of InstallSnapshotResponse is None for node {}. Trace: {}",
                                    p_target_node,
                                    TRACE
                                )
                        }
                    },
                    Err(e) => {
                        bail!(
                            "InstallSnapshotResponse for node {} contains error status {:?}. Trace: {}",
                            p_target_node,
                            e,
                            TRACE
                        )
                    }
                }
            }
            Err(e) => {
                bail!(
                    "could not create grpc channel on sending raft::InstallSnapshotResponse for node {} because: {:?}. Trace: {}",
                    p_target_node,
                    e,
                    TRACE
                )
            }
        }
    }

    /// Send an `Vote` RPC to the target Raft node
    async fn vote(&self, p_target_node: u64, p_rpc: VoteRequest) -> Result<VoteResponse> {
        const TRACE: &str = "raft_imp::vote";

        //create a channel for grpc
        let uri = format!("http://localhost:{}", BASE_PORT + p_target_node);
        match grpc::create_channel(uri).await {
            Ok(c) => {
                //call request with channel
                let uuid = Uuid::new_v5(&Uuid::NAMESPACE_X500, b"wolf_raft_vote");
                let msg_id = uuid.to_string();
                let rpc_req = raft_converter::raft_vote_req_to_grpc_vote_req(msg_id, &p_rpc);
                let compress_type = tonic::codec::CompressionEncoding::Gzip;
                let mut client = RaftClient::new(c)
                    .send_compressed(compress_type)
                    .accept_compressed(compress_type);
                match client.vote(rpc_req).await {
                    Ok(r) => match r.into_inner().response {
                        Some(s) => {
                            use wolf_raft::raft_vote_res::Response;
                            if let Response::OkRes(ok) = s {
                                let res_vote =
                                    raft_converter::grpc_vote_ok_res_to_raft_vote_res(&ok);
                                Ok(res_vote)
                            } else if let Response::ErrorRes(e) = s {
                                bail!(
                                    "VoteResponse for node {} contains Error {:?}. Trace: {}",
                                    p_target_node,
                                    e,
                                    TRACE
                                )
                            } else {
                                bail!(
                                    "VoteResponse for node {} contains Unknown error. Trace: {}",
                                    p_target_node,
                                    TRACE
                                )
                            }
                        }
                        None => {
                            bail!(
                                "inner message of VoteResponse is None for node {}. Trace: {}",
                                p_target_node,
                                TRACE
                            )
                        }
                    },
                    Err(e) => {
                        bail!(
                            "VoteResponse for node {} contains error status {:?}. Trace: {}",
                            p_target_node,
                            e,
                            TRACE
                        )
                    }
                }
            }
            Err(e) => {
                bail!(
                    "could not create grpc channel on sending raft::VoteResponse for node {} because: {:?}. Trace: {}",
                    p_target_node,
                    e,
                    TRACE)
            }
        }
    }
}

/// create a `MemRaft` node with specific node id and cluster information
/// # Panics
///
/// Will panic if config is not valid
#[must_use]
pub fn new(p_cluster_name: &str, p_node_id: u64) -> MemRaft {
    let config = Config::build(p_cluster_name.into())
        .validate()
        .unwrap_or_else(|e| {
            panic!("failed to build Raft config for cluster:{p_cluster_name} and node id:{p_node_id}. error: {e}")
        });

    //now create MemRaft node
    let arc_config = Arc::new(config);
    let network = Arc::new(RaftRouter::new());
    let storage = Arc::new(MemStore::new(p_node_id));
    raft::Raft::new(p_node_id, arc_config, network, storage)
}
