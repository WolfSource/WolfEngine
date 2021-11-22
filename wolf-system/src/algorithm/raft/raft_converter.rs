use super::raft_srv::wolf_raft::{
    ConflictOpt, RaftAppendEntriesOkRes, RaftAppendEntriesReq, RaftInstallSnapshotOkRes,
    RaftInstallSnapshotReq, RaftVoteOkRes, RaftVoteReq,
};
use anyhow::bail;

/// Convert async_raft's AppendEntriesRequest to grpc's RaftAppendEntriesReq
pub fn raft_append_entries_req_to_grpc_append_entries_req(
    p_msg_id: String,
    p_req: &async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>,
) -> anyhow::Result<RaftAppendEntriesReq> {
    const TRACE: &str =
        "raft_converter::convert_raft_append_entries_req_to_grpc_append_entries_req";
    let ret = serde_json::to_string(&p_req)
        .and_then(|json| {
            let req = RaftAppendEntriesReq {
                msg_id: p_msg_id,
                req: json,
            };
            Ok(req)
        })
        .or_else(|e| {
            bail!("could not serialize and convert async_raft::raft::AppendEntriesRequest<memstore::ClientRequest> to json because: {:?}. trace: {}", e, TRACE)
        });
    ret
}

/// Convert grpc's RaftAppendEntriesReq to async_raft's AppendEntriesRequest
pub fn grpc_append_entries_req_to_raft_append_entries_req(
    p_req: &RaftAppendEntriesReq,
) -> anyhow::Result<async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>> {
    const TRACE: &str = "raft_converter::convert_raft_append_entries_req_to_append_entries_req";
    let res: core::result::Result<
        async_raft::raft::AppendEntriesRequest<memstore::ClientRequest>,
        serde_json::Error,
    > = serde_json::from_str(&p_req.req);
    let ret = match res {
        Ok(req) => Ok(req),
        Err(e) => {
            bail!("could not deserialize json to async_raft::raft::AppendEntriesRequest<memstore::ClientRequest> to json because: {:?}. trace: {}", e, TRACE)
        }
    };
    ret
}

/// convert async_raft's AppendEntriesResponse to grpc's RaftAppendEntriesOkRes
pub fn raft_append_entries_response_to_grpc_append_entries_ok_res(
    p_msg_id: String,
    p_res: &async_raft::raft::AppendEntriesResponse,
) -> RaftAppendEntriesOkRes {
    let mut c_opt: Option<ConflictOpt> = None;
    if p_res.conflict_opt.is_some() {
        let un_opt = p_res.conflict_opt.as_ref().unwrap();
        c_opt = Some(ConflictOpt {
            term: un_opt.term,
            index: un_opt.index,
        });
    }

    let ret = RaftAppendEntriesOkRes {
        msg_id: p_msg_id,
        term: p_res.term,
        success: p_res.success,
        conflict_opt: c_opt,
    };
    ret
}

/// convert grpc's RaftAppendEntriesOkRes to async_raft's AppendEntriesResponse
pub fn grpc_append_entries_ok_res_to_raft_append_entries_res(
    p_res: &RaftAppendEntriesOkRes,
) -> async_raft::raft::AppendEntriesResponse {
    let mut c_opt: Option<async_raft::raft::ConflictOpt> = None;
    if p_res.conflict_opt.is_some() {
        let un_opt = p_res.conflict_opt.as_ref().unwrap();
        c_opt = Some(async_raft::raft::ConflictOpt {
            term: un_opt.term,
            index: un_opt.index,
        });
    }
    let ret = async_raft::raft::AppendEntriesResponse {
        term: p_res.term,
        success: p_res.success,
        conflict_opt: c_opt,
    };
    ret
}

/// Convert async_raft's InstallSnapshotRequest to grpc's RaftInstallSnapshotReq
pub fn raft_install_snapshot_req_to_grpc_install_snapshot_req(
    p_msg_id: String,
    p_req: &async_raft::raft::InstallSnapshotRequest,
) -> RaftInstallSnapshotReq {
    let ret = RaftInstallSnapshotReq {
        msg_id: p_msg_id,
        term: p_req.term,
        leader_id: p_req.leader_id,
        last_included_index: p_req.last_included_index,
        last_included_term: p_req.last_included_term,
        offset: p_req.offset,
        done: p_req.done,
        data: p_req.data.clone(),
    };
    ret
}

/// Convert grpc's RaftInstallSnapshotReq to async_raft's InstallSnapshotRequest
pub fn grpc_install_snapshot_req_to_raft_install_snapshot_req(
    p_req: &RaftInstallSnapshotReq,
) -> async_raft::raft::InstallSnapshotRequest {
    let ret = async_raft::raft::InstallSnapshotRequest {
        term: p_req.term,
        leader_id: p_req.leader_id,
        last_included_index: p_req.last_included_index,
        last_included_term: p_req.last_included_term,
        offset: p_req.offset,
        done: p_req.done,
        data: p_req.data.clone(),
    };
    ret
}

/// Convert async_raft's InstallSnapshotResponse to grpc's RaftInstallSnapshotOkRes
pub fn raft_install_snapshot_res_to_grpc_install_snapshot_ok_res(
    p_msg_id: String,
    p_req: &async_raft::raft::InstallSnapshotResponse,
) -> RaftInstallSnapshotOkRes {
    let ret = RaftInstallSnapshotOkRes {
        msg_id: p_msg_id,
        term: p_req.term,
    };
    ret
}

/// Convert grpc's RaftInstallSnapshotOkRes to async_raft's InstallSnapshotResponse
pub fn grpc_install_snapshot_ok_res_to_raft_install_snapshot_res(
    p_req: &RaftInstallSnapshotOkRes,
) -> async_raft::raft::InstallSnapshotResponse {
    let ret = async_raft::raft::InstallSnapshotResponse { term: p_req.term };
    ret
}

/// Convert async_raft's VoteRequest to grpc's RaftVoteReq
pub fn raft_vote_req_to_grpc_vote_req(
    p_msg_id: String,
    p_req: &async_raft::raft::VoteRequest,
) -> RaftVoteReq {
    let ret = RaftVoteReq {
        msg_id: p_msg_id,
        term: p_req.term,
        candidate_id: p_req.candidate_id,
        last_log_index: p_req.last_log_index,
        last_log_term: p_req.last_log_term,
    };
    ret
}

/// Convert grpc's VoteReq to async_raft's VoteRequest
pub fn grpc_vote_req_to_raft_vote_req(p_req: &RaftVoteReq) -> async_raft::raft::VoteRequest {
    let ret = async_raft::raft::VoteRequest {
        term: p_req.term,
        candidate_id: p_req.candidate_id,
        last_log_index: p_req.last_log_index,
        last_log_term: p_req.last_log_term,
    };
    ret
}

/// Convert async_raft's VoteResponse to grpc's RaftVoteOkRes
pub fn raft_vote_res_to_grpc_vote_ok_res(
    p_msg_id: String,
    p_req: &async_raft::raft::VoteResponse,
) -> RaftVoteOkRes {
    let ret = RaftVoteOkRes {
        msg_id: p_msg_id,
        term: p_req.term,
        vote_granted: p_req.vote_granted,
    };
    ret
}

/// Convert grpc's RaftVoteOkRes to async_raft's VoteResponse
pub fn grpc_vote_ok_res_to_raft_vote_res(p_req: &RaftVoteOkRes) -> async_raft::raft::VoteResponse {
    let ret = async_raft::raft::VoteResponse {
        term: p_req.term,
        vote_granted: p_req.vote_granted,
    };
    ret
}
