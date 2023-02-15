#[derive(Debug, thiserror::Error)]
pub enum RaftError {
    #[error("error on serializing a json because: `{0}`")]
    JsonSerializeError(serde_json::error::Error),
    #[error("error on deserializing a json because: `{0}`")]
    JsonDeSerializeError(serde_json::error::Error),
    #[error("error on appending enteries for node `{0}` because of: `{1}`")]
    AppendEntriesError(async_raft::NodeId, serde_json::error::Error),
}
