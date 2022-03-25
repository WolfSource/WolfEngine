use super::raft_srv::wolf_raft::{
    ConflictOpt, RaftAppendEntriesOkRes, RaftAppendEntriesReq, RaftInstallSnapshotOkRes,
    RaftInstallSnapshotReq, RaftVoteOkRes, RaftVoteReq,
};
use anyhow::{anyhow, bail};

/// Convert `async_raft`'s `AppendEntriesRequest` to grpc's `RaftAppendEntriesReq`
pub fn raft_append_entries_req_to_grpc_append_entries_req(
    p_msg_id: String,
    p_req: &async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>,
) -> anyhow::Result<RaftAppendEntriesReq> {
    const TRACE: &str =
        "raft_converter::convert_raft_append_entries_req_to_grpc_append_entries_req";

    serde_json::to_string(&p_req)
        .map(|json| {
            RaftAppendEntriesReq {
                msg_id: p_msg_id,
                req: json,
            }
        })
        .map_err(|e| {
            anyhow!("could not serialize and convert async_raft::raft::AppendEntriesRequest<memstore::ClientRequest> to json because: {:?}.", e).context(TRACE)
        })
}

/// Convert grpc's `RaftAppendEntriesReq` to `async_raft`'s `AppendEntriesRequest`
pub fn grpc_append_entries_req_to_raft_append_entries_req(
    p_req: &RaftAppendEntriesReq,
) -> anyhow::Result<async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>> {
    const TRACE: &str = "raft_converter::convert_raft_append_entries_req_to_append_entries_req";
    let app_res: Result<
        async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>,
        serde_json::Error,
    > = serde_json::from_str(&p_req.req);
    let ret = match app_res {
        Ok(aer) => Ok(aer),
        Err(e) => {
            bail!("could not deserialize json to async_raft::raft::AppendEntriesRequest<memstore::ClientRequest> to json because: {:?}. trace: {}", e, TRACE)
        }
    };
    ret
}

/// convert `async_raft`'s `AppendEntriesResponse` to grpc's `RaftAppendEntriesOkRes`
/// # Panics
///
/// Will panic if `AppendEntriesResponse` is not valid
#[must_use]
pub fn raft_append_entries_response_to_grpc_append_entries_ok_res(
    p_msg_id: String,
    p_res: &async_raft::raft::AppendEntriesResponse,
) -> RaftAppendEntriesOkRes {
    let c_opt = if p_res.conflict_opt.is_some() {
        let un_opt = p_res.conflict_opt.as_ref().unwrap();
        Some(ConflictOpt {
            term: un_opt.term,
            index: un_opt.index,
        })
    } else {
        None
    };

    RaftAppendEntriesOkRes {
        msg_id: p_msg_id,
        term: p_res.term,
        success: p_res.success,
        conflict_opt: c_opt,
    }
}

/// convert grpc's `RaftAppendEntriesOkRes` to `async_raft`'s `AppendEntriesResponse`
/// # Panics
///
/// Will panic if `RaftAppendEntriesOkRes` is not valid
#[must_use]
pub fn grpc_append_entries_ok_res_to_raft_append_entries_res(
    p_res: &RaftAppendEntriesOkRes,
) -> async_raft::raft::AppendEntriesResponse {
    let c_opt = if p_res.conflict_opt.is_some() {
        let un_opt = p_res.conflict_opt.as_ref().unwrap();
        Some(async_raft::raft::ConflictOpt {
            term: un_opt.term,
            index: un_opt.index,
        })
    } else {
        None
    };
    async_raft::raft::AppendEntriesResponse {
        term: p_res.term,
        success: p_res.success,
        conflict_opt: c_opt,
    }
}

/// Convert `async_raft`'s `InstallSnapshotRequest` to grpc's `RaftInstallSnapshotReq`
#[must_use]
pub fn raft_install_snapshot_req_to_grpc_install_snapshot_req(
    p_msg_id: String,
    p_req: &async_raft::raft::InstallSnapshotRequest,
) -> RaftInstallSnapshotReq {
    RaftInstallSnapshotReq {
        msg_id: p_msg_id,
        term: p_req.term,
        leader_id: p_req.leader_id,
        last_included_index: p_req.last_included_index,
        last_included_term: p_req.last_included_term,
        offset: p_req.offset,
        done: p_req.done,
        data: p_req.data.clone(),
    }
}

/// Convert grpc's `RaftInstallSnapshotReq` to `async_raft`'s `InstallSnapshotRequest`
#[must_use]
pub fn grpc_install_snapshot_req_to_raft_install_snapshot_req(
    p_req: &RaftInstallSnapshotReq,
) -> async_raft::raft::InstallSnapshotRequest {
    async_raft::raft::InstallSnapshotRequest {
        term: p_req.term,
        leader_id: p_req.leader_id,
        last_included_index: p_req.last_included_index,
        last_included_term: p_req.last_included_term,
        offset: p_req.offset,
        done: p_req.done,
        data: p_req.data.clone(),
    }
}

/// Convert `async_raft`'s `InstallSnapshotResponse` to grpc's `RaftInstallSnapshotOkRes`
#[must_use]
pub const fn raft_install_snapshot_res_to_grpc_install_snapshot_ok_res(
    p_msg_id: String,
    p_req: &async_raft::raft::InstallSnapshotResponse,
) -> RaftInstallSnapshotOkRes {
    RaftInstallSnapshotOkRes {
        msg_id: p_msg_id,
        term: p_req.term,
    }
}

/// Convert grpc's `RaftInstallSnapshotOkRes` to `async_raft`'s `InstallSnapshotResponse`
#[must_use]
pub const fn grpc_install_snapshot_ok_res_to_raft_install_snapshot_res(
    p_req: &RaftInstallSnapshotOkRes,
) -> async_raft::raft::InstallSnapshotResponse {
    async_raft::raft::InstallSnapshotResponse { term: p_req.term }
}

/// Convert `async_raft`'s `VoteRequest` to grpc's `RaftVoteReq`
#[must_use]
pub const fn raft_vote_req_to_grpc_vote_req(
    p_msg_id: String,
    p_req: &async_raft::raft::VoteRequest,
) -> RaftVoteReq {
    RaftVoteReq {
        msg_id: p_msg_id,
        term: p_req.term,
        candidate_id: p_req.candidate_id,
        last_log_index: p_req.last_log_index,
        last_log_term: p_req.last_log_term,
    }
}

/// Convert grpc's `VoteReq` to `async_raft`'s `VoteRequest`
#[must_use]
pub const fn grpc_vote_req_to_raft_vote_req(p_req: &RaftVoteReq) -> async_raft::raft::VoteRequest {
    async_raft::raft::VoteRequest {
        term: p_req.term,
        candidate_id: p_req.candidate_id,
        last_log_index: p_req.last_log_index,
        last_log_term: p_req.last_log_term,
    }
}

/// Convert `async_raft`'s `VoteResponse` to grpc's `RaftVoteOkRes`
#[must_use]
pub const fn raft_vote_res_to_grpc_vote_ok_res(
    p_msg_id: String,
    p_req: &async_raft::raft::VoteResponse,
) -> RaftVoteOkRes {
    RaftVoteOkRes {
        msg_id: p_msg_id,
        term: p_req.term,
        vote_granted: p_req.vote_granted,
    }
}

/// Convert grpc's `RaftVoteOkRes` to `async_raft`'s `VoteResponse`
#[must_use]
pub const fn grpc_vote_ok_res_to_raft_vote_res(
    p_req: &RaftVoteOkRes,
) -> async_raft::raft::VoteResponse {
    async_raft::raft::VoteResponse {
        term: p_req.term,
        vote_granted: p_req.vote_granted,
    }
}
